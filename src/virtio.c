#include <debug.h>
#include <virtio.h>
#include <util.h>
#include <mmu.h>
#include <pci.h>
#include <kmalloc.h> 
#include <vector.h>

static Vector *virtio_devices = NULL;

VirtioDevice *virtio_get_nth_saved_device(uint16_t n) {
    VirtioDevice *result;
    vector_get_ptr(virtio_devices, n, &result);
    return result;
}

void virtio_save_device(VirtioDevice device) {
    VirtioDevice *mem = (VirtioDevice *)kmalloc(sizeof(VirtioDevice));
    *mem = device;

    vector_push_ptr(virtio_devices, mem);
}

VirtioDevice *virtio_get_by_device(PCIDevice *pcidevice){
    for(int i = 0; i < vector_size(virtio_devices);i++){
        VirtioDevice *curr_virt_device;
        vector_get_ptr(virtio_devices,i,curr_virt_device);
        if(curr_virt_device->pcidev == pcidevice){
            return curr_virt_device;
        }
    }
    return NULL;
}

uint64_t virtio_count_saved_devices(void) {
    return vector_size(virtio_devices);
}

volatile struct VirtioCapability *virtio_get_capability(VirtioDevice *dev, uint8_t type) {
    return pci_get_virtio_capability(dev->pcidev, type);
}

/**
 * @brief Initialize the virtio system
 */


void virtio_init(void) {
    debugf("virtio_init: Initializing virtio system...\n");
    virtio_devices = vector_new();
    uint64_t num_pci_devices = pci_count_saved_devices();
    
    for (uint64_t i = 0; i < num_pci_devices; ++i) {
        PCIDevice *pcidevice = pci_get_nth_saved_device(i);

        if (pci_is_virtio_device(pcidevice)) { 
            VirtioDevice viodev;
            viodev.pcidev = pcidevice;
            viodev.common_cfg = pci_get_virtio_common_config(pcidevice);
            viodev.notify_cap = pci_get_virtio_notify_capability(pcidevice);
            viodev.isr = pci_get_virtio_isr_status(pcidevice);
            uint16_t qsize = 128;
            viodev.desc = (VirtioDescriptor *)kmalloc(VIRTIO_DESCRIPTOR_TABLE_BYTES(qsize));
            viodev.driver = (VirtioDriverRing *)kmalloc(VIRTIO_DRIVER_TABLE_BYTES(qsize));
            viodev.device = (VirtioDeviceRing *)kmalloc(VIRTIO_DEVICE_TABLE_BYTES(qsize));

            viodev.desc_idx = 0;
            viodev.driver_idx = 0;
            viodev.device_idx = 0;
            
            viodev.common_cfg->queue_desc = kernel_mmu_translate(viodev.desc);
            viodev.common_cfg->queue_driver = kernel_mmu_translate(viodev.driver);
            viodev.common_cfg->queue_device = kernel_mmu_translate(viodev.device);
            debugf("virtio_init: queue_desc = 0x%08lx physical (0x%08lx virtual)\n", viodev.common_cfg->queue_desc, viodev.desc);
            debugf("virtio_init: queue_driver = 0x%08lx physical (0x%08lx virtual)\n", viodev.common_cfg->queue_driver, viodev.driver);
            debugf("virtio_init: queue_device = 0x%08lx physical (0x%08lx virtual)\n", viodev.common_cfg->queue_device, viodev.device);
            viodev.common_cfg->queue_enable = 1;
            
            virtio_save_device(viodev);
            rng_init();
        }
    }
    debugf("virtio_init: Done initializing virtio system\n");
}

/**
 * @brief Virtio notification
 * @param viodev - virtio device to notify for
 * @param which_queue - queue number to notify
 */
void virtio_notify(VirtioDevice *viodev, uint16_t which_queue)
{
    uint16_t num_queues = viodev->common_cfg->num_queues;

    if (which_queue >= num_queues) {
        logf(LOG_ERROR, "virtio_notify: Provided queue number is too big...\n");
        return;
    }
    viodev->common_cfg->queue_select = which_queue;

    uint8_t bar_num = viodev->notify_cap->cap.bar;
    uint32_t offset = viodev->notify_cap->cap.offset;
    uint16_t queue_notify_off = viodev->common_cfg->queue_notify_off;
    uint32_t notify_off_multiplier = viodev->notify_cap->notify_off_multiplier;
    uint32_t bar = viodev->pcidev->ecam_header->type0.bar[bar_num];

    uint16_t *notify = bar + BAR_NOTIFY_CAP(offset, queue_notify_off, notify_off_multiplier);
    *notify = which_queue; 

    return;
};
