/* main.c - Application main entry point */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/printk.h>

#include "gatt.h"
#include "usb.h"
#include "led.h"

void main(void)
{
	usbStart();
	printk("#######################\n");
	printk("### USB I/O Started\n");
	printk("#######################\n");
	
	gattStart();
	printk("#######################\n");
	printk("### GATT Started\n");
	printk("#######################\n");
	
	//setConnectState(3);
	//setAdvertiseState(1);
}
