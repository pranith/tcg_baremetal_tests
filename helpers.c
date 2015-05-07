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

void power_secondary(void)
{
    int ret, cpu = 1;

    /* Sequentially power-up all secondary cores,
     * error means trying to wake up non existing cores */
    do { ret = psci_call(PSCI_CPU_ON, cpu++, ENTRY_POINT, 0); } while (!ret);
}

void power_off(void)
{
    int ret, i = 1;
    int cpu = get_cpuid();

    /* Only secondary cores should power off themselves */
    if(cpu) {
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

    /* Shut down system */
    psci_call(PSCI_SYSTEM_OFF, 0, 0, 0);
}
