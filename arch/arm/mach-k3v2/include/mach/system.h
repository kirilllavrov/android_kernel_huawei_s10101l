/*
 *  arch/arm/mach-k3v2/include/mach/system.h
 *
 *  Copyright (C) 2011 Hisilicon Co. Ltd
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __MACH_K3V2_SYSTEM_H
#define __MACH_K3V2_SYSTEM_H

#include <mach/hardware.h>
#include <mach/platform.h>
#include <asm/proc-fns.h>

extern void (*k3v2_reset)(char mode, const char *cmd);

static inline void arch_idle(void)
{
	/*
	 * This should do all the clock switching
	 * and wait for interrupt tricks
	 */
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
	/*
	 * To reset, we hit the on-board reset register
	 * in the system FPGA
	 */
	if (k3v2_reset)
		k3v2_reset(mode, cmd);
}

#endif
