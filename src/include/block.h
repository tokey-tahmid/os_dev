/*
* Block Device Driver Header
*/
#pragma once

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

#define SECTOR_SIZE             512
#define VIRTIO_BLK_T_IN         0
#define VIRTIO_BLK_T_OUT        1
#define VIRTIO_BLK_T_FLUSH      4
#define VIRTIO_BLK_T_DISCARD    11
#define VIRTIO_BLK_T_WRITE_ZEROES 13

#define VIRTIO_BLK_S_OK         0
#define VIRTIO_BLK_S_IOERR      1
#define VIRTIO_BLK_S_UNSUPP     2
#define VIRTIO_BLK_S_INVALID    111

#define VIRTIO_BLK_F_RO         5  /* Disk is read-only */
#define VIRTIO_BLK_F_SCSI       7  /* Supports scsi command passthru */
#define VIRTIO_BLK_F_CONFIG_WCE 11 /* Writeback mode available in config */
#define VIRTIO_BLK_F_MQ         12 /* Support more than one vq */
#define VIRTIO_F_ANY_LAYOUT     27
#define VIRTIO_RING_F_INDIRECT_DESC 28
#define VIRTIO_RING_F_EVENT_IDX 29

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

struct vio_blk_job {
    uint32_t id;
    uint32_t size;
    uint64_t uuid;
    struct {
        uint32_t type;
        uint32_t reserved;
        uint64_t sector;
    } header;
    const char *data;
    uint8_t status;
    bool complete;
};

typedef void (*callback_t)(struct vio_blk_job *job);