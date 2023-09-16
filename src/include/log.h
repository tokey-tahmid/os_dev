/**
 * @file log.h
 * @author Stephen Marz (sgm@utk.edu)
 * @brief Logging routines.
 * @version 0.1
 * @date 2022-05-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdbool.h>

typedef enum {
    LOG_ERROR = 1 << 0,
    LOG_DEBUG = 1 << 1,
    LOG_INFO  = 1 << 2,
    LOG_FATAL = 1 << 3,
    LOG_TEXT  = 1 << 4,
} log_type;

int  logf(log_type lt, const char *fmt, ...);
void klog(log_type lt, bool on);

#define klogon(lt)   klog(lt, true)
#define klogoff(lt)  klog(lt, false)
#define klogall()    klogon(LOG_ERROR | LOG_DEBUG | LOG_INFO | LOG_FATAL | LOG_TEXT)
#define klognone()   klogoff(LOG_ERROR | LOG_DEBUG | LOG_INFO | LOG_FATAL | LOG_TEXT)

#define errorf(fmt, ...) \
    logf(LOG_ERROR, "\033[1;31m[ERROR]\033[0m: " fmt, __VA_ARGS__)
#define error(fmt) \
    logf(LOG_ERROR, "\033[1;31m[ERROR]\033[0m: " fmt)
#define debugf(fmt, ...) \
    logf(LOG_DEBUG, "\033[1;32m[%-15s @ %4u # %-15s]\033[0m: " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define debug(fmt) \
    logf(LOG_DEBUG, "\033[1;32m[%-15s @ %4u # %-15s]\033[0m: " fmt, __FILE__, __LINE__, __func__)
#define infof(fmt, ...) \
    logf(LOG_INFO, "\033[1;35m[INFO]\033[0m: " fmt, __VA_ARGS__)
#define info(fmt) \
    logf(LOG_INFO, "\033[1;35m[INFO]\033[0m: " fmt)
#define textf(fmt, ...) \
    logf(LOG_TEXT, fmt, __VA_ARGS__)
#define text(msg) \
    logf(LOG_TEXT, msg)

#define panicf(fmt, ...) \
    logf(LOG_FATAL, "\033[1;31m{PANIC: file %s, line %u, in '%s'}\033[0m: " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); \
    __asm__ volatile("csrw sstatus,zero\n1:\nwfi\nj 1b")
#define panic_msg(msg) \
    logf(LOG_FATAL, "\033[1;31m{PANIC: file %s, line %u, in '%s'}\033[0m: " msg, __FILE__, __LINE__, __func__); \
    __asm__ volatile("csrw sstatus,zero\n1:\nwfi\nj 1b")
#define panic() \
    logf(LOG_FATAL, "\033[1;31m{PANIC: file %s, line %u, in '%s'}\033[0m\n", __FILE__, __LINE__, __func__); \
    __asm__ volatile("csrw sstatus,zero\n1:\nwfi\nj 1b")
