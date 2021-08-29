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
#include <sys/ring_buffer.h>

#include <usb/usb_device.h>

#include "gatt.h"
#include "hog.h"


#define RING_BUF_SIZE 1024
uint8_t ring_buffer[RING_BUF_SIZE];
const struct device *dev;
char sprintBuffer[128];

struct ring_buf ringbuf;

void log(char* msg)
{
char *newLine =	"\r\n";
	
	uart_fifo_fill(dev, msg, strlen(msg));
	uart_fifo_fill(dev, newLine, strlen(newLine));
}

static void interrupt_handler(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);
	
	uart_irq_update(dev);
	if(!uart_irq_is_pending(dev)) return;

	if (uart_irq_rx_ready(dev)) {
		int recv_len, prnt_pos=0;
		uint8_t buffer[64];

		recv_len = uart_fifo_read(dev, buffer, 6);
		if(recv_len != 6 || buffer[0] != 0xff) {
			sprintf(sprintBuffer, "Abort: invalid buffer data, len: %d, buffer[0]: x%02x", recv_len, buffer[0]);
			log(sprintBuffer);
			return;
		}
		
		prnt_pos += sprintf(&sprintBuffer[prnt_pos], "***Received command: ");
		
		for (int i = 0 ; i != 6 ; i++) {
			if (i) {
				prnt_pos += sprintf(&sprintBuffer[prnt_pos], ", ");
			}
			
			prnt_pos += sprintf(&sprintBuffer[prnt_pos], "x%02x", buffer[i]);
		}
		
		log(sprintBuffer);
		
		sendReport(buffer);
	}

	if (uart_irq_tx_ready(dev)) {
			uart_irq_tx_disable(dev);
	}
}

void usbStart(void)
{
	uint32_t baudrate, dtr = 0U;
	int ret;

	dev = device_get_binding("CDC_ACM_0");
	if (!dev) {
		//LOG_ERR("CDC ACM device not found");
		return;
	}

	ret = usb_enable(NULL);
	if (ret != 0) {
		//LOG_ERR("Failed to enable USB");
		return;
	}

	ring_buf_init(&ringbuf, sizeof(ring_buffer), ring_buffer);

	//LOG_INF("Wait for DTR");

	while (true) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		if (dtr) {
			break;
		} else {
			/* Give CPU resources to low priority threads. */
			k_sleep(K_MSEC(100));
		}
	}

	//LOG_INF("DTR set");

	/* They are optional, we use them to test the interrupt endpoint */
	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DCD, 1);
	if (ret) {
		//LOG_WRN("Failed to set DCD, ret code %d", ret);
	}

	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DSR, 1);
	if (ret) {
		//LOG_WRN("Failed to set DSR, ret code %d", ret);
	}

	/* Wait 1 sec for the host to do all settings */
	k_busy_wait(1000000);

	ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	if (ret) {
		//LOG_WRN("Failed to get baudrate, ret code %d", ret);
	} else {
		//LOG_INF("Baudrate detected: %d", baudrate);
	}

	uart_irq_callback_set(dev, interrupt_handler);
	uart_irq_rx_enable(dev);
	uart_irq_tx_enable(dev);
	
	/* Wait 1 sec for the host to do all settings */
	//k_busy_wait(1000000);

	log("\r\n**terminal open**");
}
