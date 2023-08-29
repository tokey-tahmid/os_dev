/**
 * @file lock.c
 * @author Stephen Marz (sgm@utk.edu)
 * @brief Mutex routines for locking/unlocking.
 * @version 0.1
 * @date 2022-05-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <csr.h>
#include <lock.h>
#include <stdint.h>

bool mutex_trylock(Mutex *mutex)
{
    return MUTEX_LOCKED != __sync_lock_test_and_set(mutex, MUTEX_LOCKED);
}

void mutex_spinlock(Mutex *mutex)
{
    while (!mutex_trylock(mutex))
        ;
}

void mutex_unlock(Mutex *mutex)
{
    __sync_lock_release(mutex);
}

void mutex_spinlock_irq_save(Mutex *mutex, int *state)
{
    uint64_t irq;
    CSR_READ(irq, "sstatus");
    *state = !!(irq & SSTATUS_SIE);
    IRQ_OFF();
    while (!mutex_trylock(mutex))
        ;
}

void mutex_unlock_irq_restore(Mutex *mutex, int state)
{
    uint64_t irq;
    mutex_unlock(mutex);
    CSR_READ(irq, "sstatus");
    CSR_WRITE("sstatus", irq | ((!!state) << SSTATUS_SIE_BIT));
}
