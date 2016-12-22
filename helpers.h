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

#ifndef HELPERS_H
#define HELPERS_H

#ifdef ARCH_ARM
#define PSCI_CPU_ON        0x84000003
#define PSCI_AFFINITY_INFO 0x84000004
#elif ARCH_AARCH64
#define PSCI_CPU_ON        0xC4000003
#define PSCI_AFFINITY_INFO 0xC4000004
#endif

#define PSCI_CPU_OFF       0x84000002
#define PSCI_SYSTEM_OFF    0x84000008

#define SYS_CFGCTRL 0x1c0100A4
#define SYS_24MHZ   0x1c01005c

#define SYS_CFGCTR_START   0x80000000
#define SYS_CFGCTR_WRITE   0x40000000
#define SYS_CFG_SHUTDOWN   0x00800000

#ifdef ATOMIC
#define LOCK   atomic_lock
#define UNLOCK atomic_unlock
#else
#define LOCK   non_atomic_lock
#define UNLOCK non_atomic_unlock
#endif

int online_cpus;
int global_lock;
int global_a;
int global_b;

int get_cpuid(void);
void secondary_power_on(void);
void secondary_power_off();

void non_atomic_lock(int *lock_var);
void non_atomic_unlock(int *lock_var);
void atomic_lock(int *lock_var);
void atomic_unlock(int *lock_var);

void printf(const char *fmt, ...);

#endif
