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

#define LOOP_SIZE 10000000

void test_spinlock()
{
    int i, errors = 0;
    int cpu = get_cpuid();

    for (i = 0; i < LOOP_SIZE; i++) {
        LOCK(&global_lock);

        if (global_a == (cpu + 1) % 2) {
            global_a = 1;
            global_b = 0;
        } else {
            global_a = 0;
            global_b = 1;
        }

        if (global_a == global_b) {
            errors++;
        }
        UNLOCK(&global_lock);
    }

    printf("CPU%d: Done - Errors: %d\n", cpu, errors);
}

void main(void)
{
    power_secondary();
    printf("CPU%d online\n", get_cpuid());

    test_spinlock();
    power_off();
}
