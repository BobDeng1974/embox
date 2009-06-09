/**
 * \file init.c
 *
 * \date May 21, 2009
 * \author anton
 */

#include "types.h"
#include "cache.h"
#include "leon.h"
#include "memory_map.h"

LEON_REGS * const l_regs = (PVOID) (RG_BASE);

void copy_data_section() {
	extern char _data_image_start, _data_start, _data_end, _bstart, _bend;

	char *src = &_data_image_start;
	char *dst = &_data_start;

	/* ROM has data at end of text; copy it. */
	while (dst < &_data_end) {
		*dst++ = *src++;
	}
	/* Zero bss */

	for (dst = &_bstart; dst < &_bend; dst++) {
		*dst = 0;
	}

}

int init() {
	//TODO during too long time for simulation:(
	copy_data_section();
	cache_data_disable();
	cache_instr_disable();

	irq_init_handlers();
	uart_init();
	timers_init();
	eth_init();//interfaces
	icmp_init();
	udp_init();

	return 0;
}
