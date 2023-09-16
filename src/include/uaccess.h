/**
 * @file uaccess.h
 * @author Stephen Marz (sgm@utk.edu)
 * @brief User access routines.
 * @version 0.1
 * @date 2022-05-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

struct page_table;
unsigned long copy_from_user(void *dst, 
                        const struct page_table *from_table, 
                        const void *from, 
                        unsigned long size);

unsigned long copy_to_user(void *to, 
                      const struct page_table *to_table, 
                      const void *src, 
                      unsigned long size);
