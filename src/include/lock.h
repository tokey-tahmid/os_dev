/**
 * @file lock.h
 * @author Stephen Marz (sgm@utk.edu)
 * @brief Mutex handling routines.
 * @version 0.1
 * @date 2022-05-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <stdbool.h>

typedef enum {
    MUTEX_UNLOCKED,
    MUTEX_LOCKED
} Mutex;

/**
 * @brief Attempt to acquire a mutex.
 * 
 * @param mutex a pointer to the mutex.
 * @return true the mutex was acquired by us.
 * @return false we could not acquire the mutex or it is
 * already locked.
 */
bool mutex_trylock(Mutex *mutex);

/**
 * @brief Keep trying a mutex until we lock it.
 * @warning This could lead to deadlocks!
 * 
 * @param mutex a pointer to the mutex to lock.
 */
void mutex_spinlock(Mutex *mutex);

/**
 * @brief Unlock a mutex unconditionally
 * 
 * @param mutex a pointer to the mutex to unlock
 */
void mutex_unlock(Mutex *mutex);

/**
 * @brief Lock a mutex but first, copies the IRQ state
 * and then turns off IRQs before trying to acquire the lock.
 * This can cause deadlocks if an IRQ is necessary to unlock.
 * 
 * @param mutex a pointer to the mutex to lock.
 * @param state an integer to store the state of the IRQ.
 * 
*/
void mutex_spinlock_irq_save(Mutex *mutex, int *state);

/**
 * @brief Unlock a mutex and then restore the IRQ state to
 * whatever is given.
 * 
 * @param mutex a pointer to the mutex to lock.
 * @param state an integer to store the state of the IRQ.
 * 
*/
void mutex_unlock_irq_restore(Mutex *mutex, int state);

