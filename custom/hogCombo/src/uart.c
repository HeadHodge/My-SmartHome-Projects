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
#include <drivers/uart.h>
#include <sys/ring_buffer.h>
#include <sys/printk.h>

#include "main.h"
#include "hog.h"

const struct device *ioPort;

const struct uart_config uartConfig = {
	.baudrate = 115200,
	.parity = UART_CFG_PARITY_NONE,
	.stop_bits = UART_CFG_STOP_BITS_1,
	.data_bits = UART_CFG_DATA_BITS_8,
	.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
};

int charCount;
uint8_t charBuffer[64];
uint32_t dtr = 0U;
char consoleBuffer[128];

void console(char *message)
{
	//printk("%s\n", message);
	//return;
	
	ring_buf_put(&ringbuf, message, strlen(message));
	ring_buf_put(&ringbuf, "\r\n", 2);
}
	
void addChar(unsigned char inputChar)
{
	if(charCount == 0 && inputChar != 0xff) {
		sprintf(consoleBuffer, "Abort addChar: invalid charBuffer[0] 0xff value: x%02x", inputChar);
		console(consoleBuffer);
		return;
	}

	charBuffer[charCount] = inputChar;
	++charCount;
	if(charCount < 6) return;
	
	int logIndex=0;
	
	sendReport(charBuffer);
	
	//logIndex += sprintf(&consoleBuffer[logIndex], "charBuffer: ");
	for(int charIndex = 0 ; charIndex != 6 ; charIndex++) {
		if(charIndex) logIndex += sprintf(&consoleBuffer[logIndex], ",");		
		logIndex += sprintf(&consoleBuffer[logIndex], "x%02x", charBuffer[charIndex]);
	}
	
	//console(consoleBuffer);
	ring_buf_put(&ringbuf, consoleBuffer, strlen(consoleBuffer));
	ring_buf_put(&ringbuf, "\r\n", 2);
	
	charCount=0;
	memset(charBuffer, 0, sizeof(charBuffer));
}

void uartStart()
{
	printk("uartStart\n");
	console("uartStart");
	unsigned char inChar = 0x00, outChar = 0x00;
	int ret;
	
	ioPort = device_get_binding("UART_0");
	
	if (!ioPort){
		sprintf(consoleBuffer, "UART_0 binding failed, ioPort: %p", ioPort);
		console(consoleBuffer);
		return;
	}
	
	ret = uart_configure(ioPort, &uartConfig);
	if (!ret){
		sprintf(consoleBuffer, "uart_configure failed, ret: %d", ret);
		console(consoleBuffer);
		return;
	}
	
	charCount=0;
	memset(charBuffer, 0, sizeof(charBuffer));
	console("** uartInput started, wait for input **");
			
	k_sleep(K_MSEC(1000));
	
	//printk("DTR: %d\n", dtr);
	
	while (true) {
		k_sleep(K_MSEC(1)); //Give CPU resources to low priority threads.
		
		//k_yield(); //Give CPU resources to low priority threads.
		//uart_line_ctrl_get(ioPort, UART_LINE_CTRL_DTR, &dtr);
		
		ret = uart_poll_in(ioPort, &inChar);
		
		if(ret == 0) {
			if(!(dtr==0 && inChar==0xaa)) addChar(inChar);
			dtr = 1;
			continue;
		}
		
		if(dtr == 0) continue;
		
		ret = ring_buf_get(&ringbuf, &outChar, 1);
		if(ret > 0) uart_poll_out(ioPort, outChar);
	}
}
