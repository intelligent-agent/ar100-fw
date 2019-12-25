/*
 * Copyright © 2017-2019 The Crust Firmware Authors.
 * SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only
 */

#ifndef DRIVERS_CLOCK_SUNXI_CCU_H
#define DRIVERS_CLOCK_SUNXI_CCU_H

#include <clock.h>
#include <device.h>
#if CONFIG_PLATFORM_A64
#include <clock/sun50i-a64-ccu.h>
#include <clock/sun8i-r-ccu.h>
#elif CONFIG_PLATFORM_A83T
#include <clock/sun8i-a83t-ccu.h>
#include <clock/sun8i-r-ccu.h>
#endif

struct sunxi_ccu {
	struct device                 dev;
	const struct sunxi_ccu_clock *clocks;
	uintptr_t                     regs;
};

extern const struct sunxi_ccu ccu;
extern const struct sunxi_ccu r_ccu;

#endif /* DRIVERS_CLOCK_SUNXI_CCU_H */
