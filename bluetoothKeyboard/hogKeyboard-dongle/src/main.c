/* main.c - Application main entry point */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gatt.h"
#include "usb.h"
#include "led.h"

void main(void)
{
	usbStart();
	
	log("start HoG Combo");
	gattStart();
}
