#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdint.h>
#include "virtio.h"

#define SECTOR_SIZE        512
#define VIRTIO_BLK_T_IN    0
#define VIRTIO_BLK_T_OUT   1
#define VIRTIO_BLK_T_FLUSH 4

struct block_header {
    uint32_t type;
    uint32_t reserved;
    uint64_t sector;
};

struct virtio_blk_config {
    uint64_t capacity;
    uint32_t size_max;
    uint32_t seg_max;
    struct virtio_blk_geometry {
        uint16_t cylinders;
        uint8_t  heads;
        uint8_t  sectors;
    } geometry;
    uint32_t blk_size;
    struct virtio_blk_topology {
        uint8_t  physical_block_exp;
        uint8_t  alignment_offset;
        uint16_t min_io_size;
        uint32_t opt_io_size;
    } topology;
    uint8_t  writeback;
    uint8_t  unused0[3];
    uint32_t max_discard_sectors;
    uint32_t max_discard_seg;
    uint32_t discard_sector_alignment;
    uint32_t max_write_zeroes_sectors;
    uint32_t max_write_zeroes_seg;
    uint8_t  write_zeroes_may_unmap;
    uint8_t  unused1[3];
};

void block_init(struct virtio_device *dev);
int block_read(uint64_t sector, uint8_t *buffer, uint32_t size);
int block_write(uint64_t sector, uint8_t *buffer, uint32_t size);

#endif
