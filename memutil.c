/*
 * memutil: Simple program to read/write from/to any location in memory.
 *
 *  Copyright (C) 2000, Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *  Copyright (C) 2016, Huy Quoc Duong (hduong@apm.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdint.h>

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

int main(int argc, char **argv) {
	int mem_fd;
	void *mem_map;
	volatile uint8_t *virt_addr;
	uint32_t map_size, map_mask;
	uint32_t read_result, writeval;
	off_t target;
	int access_type = 'w';

	if(argc < 2) {
		fprintf(stderr, "\nUsage:\t%s { address } [ type [ data ] ]\n"
			"\taddress : memory address to act upon\n"
			"\ttype    : access operation type : [b]yte, [h]alfword, [w]ord\n"
			"\tdata    : data to be written\n\n",
			argv[0]);
		exit(1);
	}

	target = strtoul(argv[1], 0, 0);

	if(argc > 2)
		access_type = tolower(argv[2][0]);

	if((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
	fflush(stdout);

	map_size = getpagesize();
	map_mask = map_size - 1;

	// Now map it
	mem_map = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, target & ~map_mask);
	fflush(stdout);

	virt_addr = (volatile uint8_t *) mem_map + (target & map_mask);

	switch(access_type) {
	case 'b':
		read_result = *((uint8_t *) virt_addr);
		break;
	case 'h':
		read_result = *((uint16_t *) virt_addr);
		break;
	case 'w':
		read_result = *((uint32_t *) virt_addr);
		break;
	default:
		fprintf(stderr, "Illegal data type '%c'.\n", access_type);
		exit(2);
	}

	printf("Value at address 0x%x (%p): 0x%x\n", target, virt_addr, read_result);
	fflush(stdout);

	if(argc > 3) {
		writeval = strtoul(argv[3], 0, 0);
		switch(access_type) {
		case 'b':
			*((uint8_t *) virt_addr) = writeval;
			read_result = *((uint8_t *) virt_addr);
			break;
		case 'h':
			*((uint16_t *) virt_addr) = writeval;
			read_result = *((uint16_t *) virt_addr);
			break;
		case 'w':
			*((uint32_t *) virt_addr) = writeval;
			read_result = *((uint32_t *) virt_addr);
			break;
		}
		printf("Written 0x%x; readback 0x%x\n", writeval, read_result);
		fflush(stdout);
	}
	if(munmap((void *)mem_map, map_size) == -1) FATAL;
	close(mem_fd);
	return 0;
}
