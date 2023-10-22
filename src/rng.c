/*
*   Random Number Generator Driver
*/
#pragma once

#include <pci.h>
#include <virtio.h>
#include <stdbool.h>
#include <debug.h>
#include <mmu.h>
#include <kmalloc.h>
#include <rng.h>

static Vector *rng_active_jobs;

void rng_init(){
    rng_active_jobs = vector_new();
}

void rng_get_numbers(struct VirtioDevice *viodev, void *buffer, uint16_t size, void* callback){

    if(viodev->pcidev->ecam_header->device_id != VIRTIO_PCI_DEVICE_ID(VIRTIO_PCI_DEVICE_ENTROPY)){
        logf(LOG_ERROR,"[RNG] Incorrect device provided\n");
    }
    struct RNGJob *job;

    job->assigned_device = viodev;
    job->callback = callback;
    job->output = buffer;
    
    vector_push(rng_active_jobs,job);
    if(!rng_fill(buffer,size,viodev)){
        logf(LOG_ERROR,"[RNG] Failed to generate random numbers\n");
    }
}

bool rng_fill(void *buffer, uint16_t size, struct VirtioDevice *rng_device) {
    uint64_t phys;
    uint32_t at_idx;
    uint32_t mod;

    mod = rng_device->common_cfg->queue_size;
    at_idx = rng_device->desc_idx;
    uint64_t virt = kzalloc(size);
    phys = mmu_translate(virt, (uint64_t)buffer);

    rng_device->desc[at_idx].addr = phys;
    rng_device->desc[at_idx].len = size;
    rng_device->desc[at_idx].flags = VIRTQ_DESC_F_WRITE;
    rng_device->desc[at_idx].next = 0;
    rng_device->driver->ring[rng_device->driver->idx % mod] = at_idx;

    rng_device->driver->idx += 1;
    rng_device->desc_idx = (rng_device->desc_idx + 1) % mod;

    virtio_notify(rng_device,at_idx);

    return true;
}

void rng_job_done(VirtioDevice *vertdevice){
    RNGJob *job;
    int idx;
    for(int i = 0; i < vector_size(rng_active_jobs);i++){
        vector_get_ptr(rng_active_jobs,i,job);
        if(job->assigned_device == vertdevice){
            vector_remove(rng_active_jobs,i);
            break;
        }
        else{
            job = NULL;
        }
    }

    if(job != NULL){

        Callback callback_func = job->callback;
        callback_func(job->output);
    }
}
