#include <stdint.h>
#include <pci.h>
#include <virtio.h>
#include <stdbool.h>
#include <debug.h>
#include <util.h>
#include <mmu.h>
#include <kmalloc.h>
#include <elf.h>
#include <process.h>
#include <rng.h>
#include <block.h>
#include <stdlib.h>
#include <map.h>

static Vector *vio_blk_devices;

static void vio_blk_finish(VirtioDevice *vdev, uint16_t major) {
    struct vio_blk_job *job;
    uint64_t id = major;

    if (map_get_int(vdev->jobs, id, (void **)&job)) {
        if (job->callback) {
            job->callback(job);
        }
        free(job);
    }
}

static void vio_blk_request(VirtioDevice *blkdev, struct vio_blk_job *job) {
    uint32_t at_idx;
    uint32_t mod = blkdev->common_cfg->queue_size;

    at_idx = blkdev->desc_idx;

    // Populate the descriptor chain
    // 1. Block header
    blkdev->desc[at_idx].addr = mmu_translate(blkdev->page_table, (uintptr_t)&job->header);
    blkdev->desc[at_idx].len = sizeof(struct block_header);
    blkdev->desc[at_idx].flags = VIRTQ_DESC_F_NEXT;
    blkdev->desc[at_idx].next = at_idx + 1;

    // 2. Data buffer
    at_idx = (at_idx + 1) % mod;
    blkdev->desc[at_idx].addr = mmu_translate(blkdev->page_table, (uintptr_t)&job->header);
    blkdev->desc[at_idx].len = job->size;
    blkdev->desc[at_idx].flags = (job->header.type == VIRTIO_BLK_T_OUT) ? VIRTQ_DESC_F_WRITE : 0;
    blkdev->desc[at_idx].flags |= VIRTQ_DESC_F_NEXT;
    blkdev->desc[at_idx].next = at_idx + 1;

    // 3. Status byte
    at_idx = (at_idx + 1) % mod;
    blkdev->desc[at_idx].addr = mmu_translate(blkdev->page_table, (uintptr_t)&job->header);
    blkdev->desc[at_idx].len = sizeof(uint8_t);
    blkdev->desc[at_idx].flags = VIRTQ_DESC_F_WRITE;
    blkdev->desc[at_idx].next = 0;

    // Update the available ring
    blkdev->driver->ring[blkdev->driver->idx % mod] = blkdev->desc_idx;
    blkdev->driver->idx += 1;

    // Notify the device
    virtio_notify(blkdev, blkdev->desc_idx);

    // Update the descriptor index
    blkdev->desc_idx = (at_idx + 1) % mod;
}

void vio_blk_init() {
    // Initialize vio_blk_devices and any other necessary structures
    vio_blk_devices = vector_new();
}

void vio_blk_read(VirtioDevice *blkdev, uint64_t sector, uint32_t count, void *buffer, callback_t callback) {
    struct vio_blk_job *job = malloc(sizeof(struct vio_blk_job));
    if (!job) {
        // Handle memory allocation failure
        return;
    }
    struct vio_blk_job *job = malloc(sizeof(struct vio_blk_job));
    job->header.type = VIRTIO_BLK_T_IN;
    job->header.sector = sector;
    job->size = count * SECTOR_SIZE;
    job->data = buffer;
    job->complete = false;
    job->callback = callback;

    vio_blk_request(blkdev, job);
}

void vio_blk_write(VirtioDevice *blkdev, uint64_t sector, uint32_t count, const void *buffer, callback_t callback) {
    struct vio_blk_job *job = malloc(sizeof(struct vio_blk_job));
    if (!job) {
        // Handle memory allocation failure
        return;
    }
    struct vio_blk_job *job = malloc(sizeof(struct vio_blk_job));
    job->header.type = VIRTIO_BLK_T_OUT;
    job->header.sector = sector;
    job->size = count * SECTOR_SIZE;
    job->data = buffer;
    job->complete = false;
    job->callback = callback;

    vio_blk_request(blkdev, job);
}