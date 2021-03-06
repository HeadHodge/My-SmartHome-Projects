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

#include "hog.h"

const struct device *comPort;
char logBuffer[128];

void log(char* msg)
{
	sprintf(logBuffer, "%s\r\n", msg);
	uart_fifo_fill(comPort, logBuffer, strlen(logBuffer));
}

static void interrupt_handler(const struct device *comPort, void *user_data)
{
	ARG_UNUSED(user_data);
	uart_irq_update(comPort);

	//if (uart_irq_tx_ready(comPort)) uart_irq_tx_disable(comPort);
	//if (!uart_irq_rx_ready(comPort)) return;

	int recv_len, prnt_pos=0;
	uint8_t buffer[64];

	memset(buffer, 0, sizeof(buffer));
	recv_len = uart_fifo_read(comPort, buffer, sizeof(buffer));
	
	prnt_pos += sprintf(&logBuffer[prnt_pos], "***Received command: ");
	
	for (int i = 0 ; i != 6 ; i++) {
		if (i) prnt_pos += sprintf(&logBuffer[prnt_pos], ", ");		
		prnt_pos += sprintf(&logBuffer[prnt_pos], "x%02x", buffer[i]);
	}
	
	log(logBuffer);
	
	if(recv_len != 6 || buffer[0] != 0xff) {
		sprintf(logBuffer, "Abort: invalid buffer data, len: %d, buffer: %s", recv_len, buffer);
		log(logBuffer);
		return;
	}
	
	sendReport(buffer);
}

void usbStart(void)
{
	uint32_t baudrate, dtr = 0U;
	int ret;

	comPort = device_get_binding("CDC_ACM_0");
	if (!comPort) return;
	if (usb_enable(NULL) != 0) return;

	uart_irq_callback_set(comPort, interrupt_handler);
	uart_irq_rx_enable(comPort);

	/* Wait 1 sec for the host to do all settings */
	k_busy_wait(1000000);

	log("Wait for DTR");

	while (true) {
		uart_line_ctrl_get(comPort, UART_LINE_CTRL_DTR, &dtr);

		if (dtr)
			break;
		else
			k_sleep(K_MSEC(100)); //Give CPU resources to low priority threads.
	}

	log("DTR is set");

	/* They are optional, we use them to test the interrupt endpoint */
	ret = uart_line_ctrl_set(comPort, UART_LINE_CTRL_DCD, 1);
	if (ret) log("Failed to set DCD");

	ret = uart_line_ctrl_set(comPort, UART_LINE_CTRL_DSR, 1);
	if (ret) log("Failed to set DSR");

	ret = uart_line_ctrl_get(comPort, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	if (ret) {
		log("Failed to get baudrate");
	} else {
		log("Baudrate detected");
	}
	
	log("*** Terminal Open ***");
}
