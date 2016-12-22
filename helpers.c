/*
 * Copyright (C) 2015 Virtual Open Systems SAS
 * Author: Alexander Spyridakis <a.spyridakis@virtualopensystems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "helpers.h"

#ifdef ARCH_ARM
__asm__(".arch_extension virt");

int get_cpuid(void)
{
    int cpu;
    asm volatile ("mrc p15, 0, %0, c0, c0, 5; and %0, %0, #15\n" : "=r"(cpu));
    return cpu;
}

int psci_call(int psci_function, int arg0, int arg1, int arg2)
{
    int ret;
    asm volatile ("hvc #0; mov %0, r0\n" : "=r" (ret));
    return ret;
}
#elif ARCH_AARCH64
int get_cpuid(void)
{
    int cpu;
    asm volatile ("mrs %0, MPIDR_EL1; and %0, %0, #15\n" : "=r"(cpu));
    return cpu;
}

int psci_call(int psci_function, int arg0, int arg1, int arg2)
{
    int ret;
    asm volatile ("hvc #0; mov %0, x0\n" : "=r" (ret));
    return ret;
}
#endif

#ifndef VEXPRESS
void secondary_power_on(void)
{
    int ret, cpu = 1;

    online_cpus++;
    if(get_cpuid()) {
        return;
    }

    /* Sequentially power-up all secondary cores,
     * error means trying to wake up non existing cores */
    do { ret = psci_call(PSCI_CPU_ON, cpu++, ENTRY_POINT, 0); } while (!ret);

    /* Wait for all secondaries to wakeup */
    while(online_cpus != (cpu - 1));
}
#else
void secondary_power_on(void)
{
    int *sys_24mhz = (int *)SYS_24MHZ;

    online_cpus++;

    /* Wait 2 second for any secondaries */
    while(*sys_24mhz <= 48000000);
}
#endif

#ifndef VEXPRESS
void secondary_power_off(void)
{
    int ret, i = 1;

    /* Only secondary cores should power off themselves */
    if(get_cpuid()) {
        online_cpus--;
        psci_call(PSCI_CPU_OFF, 0, 0, 0);
        return;
    }

    /* Primary core should wait for all secondaries to be powered off */
    do {
        ret = psci_call(PSCI_AFFINITY_INFO, i, 0, 0);

        /* If a core was powered off, wait for the next one */
        if (ret == 1) {
            i++;
        }
    } while(ret >= 0);
}

void shut_down(void)
{
    /* Shut down system */
    psci_call(PSCI_SYSTEM_OFF, 0, 0, 0);
}
#else
void secondary_power_off(void)
{
    int *sys_cfgctrl = (int *)(SYS_CFGCTRL);

    online_cpus--;

    if(get_cpuid()) {
        asm("wfi");
    }

    /* Wait for any secondary cores */
    while(online_cpus);
}

void shut_down(void)
{
    int *sys_cfgctrl = (int *)(SYS_CFGCTRL);

    /* Shutdown system */
    *sys_cfgctrl = SYS_CFGCTR_START | SYS_CFGCTR_WRITE | SYS_CFG_SHUTDOWN;
}
#endif

void atomic_lock(int *lock_var)
{
    while (__sync_lock_test_and_set(lock_var, 1));
}

void atomic_unlock(int *lock_var)
{
    __sync_lock_release(lock_var);
}

void non_atomic_lock(int *lock_var)
{
    while (*lock_var != 0);
    *lock_var = 1;
}

void non_atomic_unlock(int *lock_var)
{
    *lock_var = 0;
}
