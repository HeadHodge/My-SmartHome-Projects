#include <zephyr.h>
#include <stdio.h>
//#include <sys/printk.h>

#include "ble.h"
#include "uart.h"

void main(void)
{
	int err;
	printf("Start smartRemotes Ble Input\n");
	
	err = uartStart();
	
	if (err) {
		printf("uartStart failed (err %d)\n", err);
		return;
	}

	err = bleStart();
	
	if (err) {
		printf("bleStart failed (err %d)\n", err);
		return;
	}

	
	// Implement notification. At the moment there is no suitable way
	// of starting delayed work so we do it here
	
	while (1) {
		k_sleep(K_MSEC(10));
		
		//k_sleep(K_SECONDS(5));
		//k_cpu_idle();		

		//Battery level simulation 
		//bas_notify();
		
		//Get uart input for hid host reports
		checkInput();

	}
}
