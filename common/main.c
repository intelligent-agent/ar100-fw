/*
 * Copyright © 2017-2020 The Crust Firmware Authors.
 * SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only
 */

#include <css.h>
#include <debug.h>
#include <stddef.h>
#include <spr.h>
#include <system.h>
#include <gpio.h>
#include <gpio/sunxi-gpio.h>
#include <platform/devices.h>
#include <platform/memory.h>
#include <mmio.h>
#include <delay.h>


noreturn void main(uint32_t exception);
noreturn void
main(uint32_t exception)
{
	static const struct gpio_handle pins[] = {
      {
				.dev   = &pio.dev,
				.id    = SUNXI_GPIO_PIN(4, 14),
				.drive = DRIVE_10mA,
				.mode  = 1,
				.pull  = PULL_UP,
      },
       {
 				.dev   = &pio.dev,
 				.id    = SUNXI_GPIO_PIN(4, 0),
 				.drive = DRIVE_10mA,
 				.mode  = 1,
 				.pull  = PULL_UP,
 	     },
       {
  				.dev   = &pio.dev,
  				.id    = SUNXI_GPIO_PIN(4, 8),
  				.drive = DRIVE_10mA,
  				.mode  = 1,
  				.pull  = PULL_UP,
  	   }
  };
	int err;
	uint32_t pin_state, wait_cycles;
	if (exception) {
		error("Unhandled exception %u at %p!",
		      exception, (void *)mfspr(SPR_SYS_EPCR_ADDR(0)));
	}

	/* Swith CPUS to 300 MHz. This should be done in Linux eventually */
	mmio_write_32(DEV_R_PRCM, 2<<16 | 1<<8);

	css_init();

  for(int i=0; i<3; i++){
    if ((err = gpio_get(&pins[i]))){
      error("Could not get pin \n");
    }
  }

	for(int i=0; i<SCPI_MEM_SIZE; i+=16){
    mmio_write_32(SCPI_MEM_BASE + i, 0xFFFFFFFF);
    mmio_write_32(SCPI_MEM_BASE + i+4, 0x0);
    mmio_write_32(SCPI_MEM_BASE + i+8, 0x0);
    mmio_write_32(SCPI_MEM_BASE + i+12, 0x0);
	}
	while(1){
		for(int i=0; i<SCPI_MEM_SIZE; i+=8){
			pin_state = mmio_read_32(SCPI_MEM_BASE + i);
			wait_cycles = mmio_read_32(SCPI_MEM_BASE + i+4);
			mmio_write_32(DEV_PIO + (4*0x24) +  0x10, pin_state);
			udelay(wait_cycles);
		}
	};
}
