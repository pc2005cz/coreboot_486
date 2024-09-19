/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <cpu/cpu.h>
#include <delay.h>
#include <stdint.h>
#include <thread.h>

#define CLOCK_TICK_RATE	1193180U /* Underlying HZ */


void init_timer(void)
{
	printk(BIOS_DEBUG, "Init PIT timer\n");
}


void udelay(unsigned int us)
{
	if (!thread_yield_microseconds(us))
		return;

	if (us ==0 ) return;

	/* Set the Gate high, disable speaker */
	outb((inb(0x61) & ~0x02) | 0x01, 0x61);

	outb(0xb0, 0x43);	/* binary, mode 0, LSB/MSB, Ch 2 */

	do {
		unsigned int current_us = (us > 60000) ? 60000 : us;

		outb(current_us & 0xff, 0x42);	/* LSB of count */
		outb(current_us	>> 8, 0x42);	/* MSB of count */

		while (!(inb(0x61) & (1<<5))) {
			cpu_relax();
		}

		us -= current_us;
	} while (us > 0);
}

#if CONFIG(PIT_MONOTONIC_TIMER)
#include <timer.h>

static struct monotonic_counter {
	int initialized;
	struct mono_time time;
	uint64_t last_value;
} mono_counter;

void timer_monotonic_get(struct mono_time *mt)
{

	*mt = 0;
#if 0
	uint64_t current_tick;
	uint64_t ticks_elapsed;
	unsigned long ticks_per_usec;

	if (!mono_counter.initialized) {
		init_timer();
		mono_counter.last_value = rdtscll();
		mono_counter.initialized = 1;
	}

	current_tick = rdtscll();
	ticks_elapsed = current_tick - mono_counter.last_value;
	ticks_per_usec = tsc_freq_mhz();

	/* Update current time and tick values only if a full tick occurred. */
	if (ticks_elapsed >= ticks_per_usec) {
		uint64_t usecs_elapsed;

		usecs_elapsed = ticks_elapsed / ticks_per_usec;
		mono_time_add_usecs(&mono_counter.time, (long)usecs_elapsed);
		mono_counter.last_value = current_tick;
	}

	/* Save result. */
	*mt = mono_counter.time;
#endif
}
#endif
