/*
 * Copyright (c) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Sample echo app for CDC ACM class
 *
 * Sample app for USB CDC ACM class driver. The received data is echoed back
 * to the serial port.
 */

#include <stdio.h>
#include <string.h>
#include <device.h>
#include <drivers/uart.h>
#include <zephyr.h>

#include <usb/usb_device.h>

//#include "gatt.h"
#include "hog.h"

const struct device *comPort;

static void interrupt_handler(const struct device *comPort, void *user_data)
{
	ARG_UNUSED(user_data);

	uart_irq_update(comPort);

	if (!uart_irq_rx_ready(comPort)) return;

	int recv_len;
	uint8_t buffer[64];

	recv_len = uart_fifo_read(comPort, buffer, 6);
	
	if(recv_len != 6 || buffer[0] != 0xff) {
		printk("Abort: invalid buffer data, len: %d, buffer[0]: x%02x\n", recv_len, buffer[0]);
		return;
	}
	
	printk("***Received command: ");
	
	for (int i = 0 ; i != 6 ; i++) {
		if (i) printk(", ");
		printk("x%02x", buffer[i]);
	}
	
	printk("\n");
	
	sendReport(buffer);
}

void usbStart(void)
{
	uint32_t baudrate = 0, dtr = 0U;
	int ret;

	comPort = device_get_binding("CDC_ACM_0");
	if (!comPort) {
		printk("CDC ACM device not found\n");
		return;
	}

	ret = usb_enable(NULL);
	if (ret != 0) {
		printk("Failed to enable USB\n");
		return;
	}
	
	uart_irq_callback_set(comPort, interrupt_handler);
	uart_irq_rx_enable(comPort);
	uart_irq_tx_enable(comPort);
	
	/* Wait 1 sec for the host to do all settings */
	k_busy_wait(10000000);

	printk("Wait for DTR\n");

	while (true) {
		uart_line_ctrl_get(comPort, UART_LINE_CTRL_DTR, &dtr);
		
		if (dtr) {
			break;
		} else {
			/* Give CPU resources to low priority threads. */
			k_sleep(K_MSEC(100));
		}
	}

	printk("DTR set\n");

	/* They are optional, we use them to test the interrupt endpoint */
	ret = uart_line_ctrl_set(comPort, UART_LINE_CTRL_DCD, 1);
	if (ret) {
		printk("Failed to set DCD, ret code %d\n", ret);
	}

	ret = uart_line_ctrl_set(comPort, UART_LINE_CTRL_DSR, 1);
	if (ret) {
		printk("Failed to set DSR, ret code %d\n", ret);
	}

	/* Wait 1 sec for the host to do all settings */
	k_busy_wait(1000000);

	ret = uart_line_ctrl_get(comPort, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	
	if (ret) {
		printk("Failed to get baudrate, ret code %d\n", ret);
	} else {
		printk("Baudrate detected: %d\n", baudrate);
	}

	printk("*** Terminal Open ***\n");
}
