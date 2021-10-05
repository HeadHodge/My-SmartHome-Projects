#include <drivers/uart.h>
#include <sys/ring_buffer.h>
#include <stdio.h>

#include "main.h"
#include "hids.h"

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

/*
char consoleBuffer[128];

void console(char *message)
{
	printf("%s\n", message);
	return;
	
	ring_buf_put(&ringbuf, message, strlen(message));
	ring_buf_put(&ringbuf, "\r\n", 2);
}
*/

void writeMsg(char *msg)
{
	unsigned int charCnt=0, maxCnt=strlen(msg);
	
	while(charCnt < maxCnt){
		uart_poll_out(ioPort, msg[charCnt]);
		++charCnt;
	}
	
	uart_poll_out(ioPort, '\r');
	uart_poll_out(ioPort, '\n');
}
	
void buildReport(unsigned char inputChar)
{
	//uint8_t msg[64];
	
	//sprintf(msg, "x%02x", inputChar);
	//writeMsg(msg);
	
	if(charCount == 0 && inputChar != 0xff) {
		printf("\nAbort buildReport: invalid sync char: x%02x", inputChar);
		return;
	}

	charBuffer[charCount] = inputChar;
	++charCount;
	if(charCount < 6) return;
	
	sendReport(charBuffer);
	k_sleep(K_MSEC(100));
	charBuffer[2] = 0;
	charBuffer[3] = 0;
	sendReport(charBuffer);
	
	
	memset(charBuffer, 0, sizeof(charBuffer));
	charCount=0;
}

void checkInput()
{
	unsigned char inChar;
	
	if(uart_poll_in(ioPort, &inChar) != 0) return;

	buildReport(inChar);
}

int uartStart()
{
	ioPort = device_get_binding("UART_0");
	
	if (!ioPort){
		printf("UART_1 binding failed, ioPort: %p", ioPort);
		return -1;
	}

	printf("Uart initialized\n");
	writeMsg("Hello");
	return 0;
}
