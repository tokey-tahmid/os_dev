/**
 * @file device.h
 * @author Stephen Marz (sgm@utk.edu)
 * @brief Device control and structures.
 * @version 0.1
 * @date 2022-05-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#define DEVICE_MAJOR_INPUT        2
#define DEVICE_MAJOR_ENTROPY      3
#define DEVICE_MAJOR_BLOCK_START  8
#define DEVICE_MAJOR_FRAMEBUFFER  210

struct file;
struct inode;

struct fileops {
    int (*open)(struct file *fl, struct inode *ino);
    int (*close)(struct file *fl);
    int (*read)(struct file *fl, char *buffer, int start, int end);
    int (*write)(struct file *fl, const char *buffer, int start, int end);
    int (*flush)(struct file *fl);
    int (*ioctl)(struct file *fl, int cmd, char *buffer);
};

