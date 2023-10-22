#include "block.h"
#include "virtio.h"
#include "pci.h"
#include "mmu.h"
#include <stddef.h>

static struct virtio_blk_config blk_config;

void block_init() {
    // Initialize PCI and get the device
    struct pci_dev *dev = pci_init();
    if (dev->vendorid == 0x1AF4) {
        virtio_init(dev);
    }

    // Initialize the block device
    // You might need to read the blk_config from the device configuration section
}

void block_read(uint64_t sector, void* buffer, uint32_t size) {
    struct block_header header = {
        .type = VIRTIO_BLK_T_IN,
        .reserved = 0,
        .sector = sector
    };

    // Convert virtual address to physical address
    uint64_t paddr = mmu_translate((uint64_t)buffer);

    // Setup the descriptors and rings
    // ...

    // Notify the device
    // ...
}

void block_write(uint64_t sector, void* buffer, uint32_t size) {
    struct block_header header = {
        .type = VIRTIO_BLK_T_OUT,
        .reserved = 0,
        .sector = sector
    };

    // Convert virtual address to physical address
    uint64_t paddr = mmu_translate((uint64_t)buffer);

    // Setup the descriptors and rings
    // ...

    // Notify the device
    // ...
}
