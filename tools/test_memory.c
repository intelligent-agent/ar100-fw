/*
 * Copyright © 2017-2020 The Crust Firmware Authors.
 * SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only
 */

#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <math.h>

#include <config.h>
#include <kconfig.h>
#include <mmio.h>
#include <util.h>
#include <asm/exception.h>
#include <platform/devices.h>
#include <platform/memory.h>

#ifndef PAGESIZE
#define PAGESIZE          0x1000
#endif
#define PAGE_BASE(addr)   ((addr) & ~(PAGESIZE - 1))
#define PAGE_OFFSET(addr) ((addr) & (PAGESIZE - 1))

int
main(void)
{
	char *sram;
	int fd;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("Failed to open /dev/mem");
		return EXIT_FAILURE;
	}
	sram = mmap(NULL, SRAM_A2_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
	            fd, PAGE_BASE(SRAM_A2_OFFSET + SRAM_A2_BASE));
	if (sram == MAP_FAILED) {
		perror("Failed to mmap SRAM A2");
		return EXIT_FAILURE;
	}
	close(fd);

	uint32_t test_pattern[SCPI_MEM_SIZE/4];
	float amax = SCPI_MEM_SIZE/8;
	for(int i=0; i<SCPI_MEM_SIZE/8; i+=4){
		test_pattern[i] 	= 0xFFFF;
		test_pattern[i+1] = 3000 - 2000*sin(3.14*i/amax);
		test_pattern[i+2] = 0xFF00;
		test_pattern[i+3] = 3000 - 2000*sin(3.14*i/amax);
	}
	for(int i=SCPI_MEM_SIZE/8; i<SCPI_MEM_SIZE/4; i+=4){
		test_pattern[i] 	= 0x00FF;
		test_pattern[i+1] = 3000 - 2000*sin(3.14*(i-amax)/amax);
		test_pattern[i+2] = 0x0000;
		test_pattern[i+3] = 3000 - 2000*sin(3.14*(i-amax)/amax);
	}
	printf("Writing test pattern (%jd/%d bytes used)\n",
	       SCPI_MEM_SIZE, SCPI_MEM_SIZE);
	memcpy(sram + SCPI_MEM_BASE, test_pattern, SCPI_MEM_SIZE);
	msync(sram + SCPI_MEM_BASE, SCPI_MEM_SIZE, MS_SYNC);
	munmap(sram, SRAM_A2_SIZE);

	return EXIT_SUCCESS;
}
