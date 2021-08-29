/** @file
 *  @brief HoG Service sample
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "main.h"
#include "gatt.h"
#include "uart.h"

enum {
    HIDS_REMOTE_WAKE = BIT(0),
    HIDS_NORMALLY_CONNECTABLE = BIT(1),
};

struct hids_info {
    uint16_t version; /* version number of base USB HID Specification */
    uint8_t code; /* country HID Device hardware is localized for. */
    uint8_t flags;
} __packed;

struct hids_report_reference {
    uint8_t id; /* report id */
    uint8_t type; /* report type */
} __packed;

static struct hids_info info = {
    .version = 0x0000,
    .code = 0x00,
    .flags = HIDS_NORMALLY_CONNECTABLE,
};

static uint8_t ctrl_point;

static uint8_t report_map[] = {
	//Report Reference Id = 1 (128 key Keyboard)
	//Modifier Byte: bit 0 is L CTRL, bit 1 is L SHIFT, bit 2 is L ALT, bit 3 is L GUI, bit 4 is R CTRL, bit 5 is R SHIFT, bit 6 is R ALT, and bit 7 is R GUI
	0x05, 0x01, 	  // Usage Page (Generic Desktop Ctrls)
	0x09, 0x06, 	  // Usage (Keyboard)
	0xa1, 0x01, 	  // Collection (Application)
	0x85, 0x01, 	  //   Report ID (1)
	0x05, 0x07, 	  //   Usage Page (Kbrd/Keypad)
	0x19, 0xe0, 	  //   Usage Minimum (0xE0)
	0x29, 0xe7, 	  //   Usage Maximum (0xE7)
	0x15, 0x00, 	  //   Logical Minimum (0)
	0x25, 0x01, 	  //   Logical Maximum (1)
	0x75, 0x01, 	  //   Report Size (1)
	0x95, 0x08, 	  //   Report Count (8)
	0x81, 0x02, 	  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x95, 0x01, 	  //   Report Count (1)
	0x75, 0x08, 	  //   Report Size (8)
	0x15, 0x00, 	  //   Logical Minimum (0)
	0x25, 0x65, 	  //   Logical Maximum (101)
	0x05, 0x07, 	  //   Usage Page (Kbrd/Keypad)
	0x19, 0x00, 	  //   Usage Minimum (0x00)
	0x29, 0x65, 	  //   Usage Maximum (0x65)
	0x81, 0x00, 	  //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xc0,			  // End Collection

	//Report Reference Id = 2 (Consumer Entertainment Control)
	0x05, 0x0C, 	  // Usage Page (Consumer)
	0x09, 0x01, 	  // Usage (Consumer Control)
	0xA1, 0x01, 	  // Collection (Application)
	0x85, 0x02, 	  //   Report ID (2)
	0x75, 0x10, 	  //   Report Size (16)
	0x95, 0x01, 	  //   Report Count (1)
	0x15, 0x01,		  //   Logical Minimum (1)
	0x26, 0xff, 0x07, //   Logical Maximum (2047)
	0x19, 0x01, 	  //   Usage Minimum (Consumer Control)
	0x2A, 0xff, 0x07, //   Usage Maximum (0x07FF)
	0x81, 0x00, 	  //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0, 	  	  	  // End Collection

	//Report Reference Id = 3 (Generic Mouse)
	0x05, 0x01,       // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,       // Usage (Mouse)
    0xA1, 0x01,       // Collection (Application)
	0x85, 0x03, 	  //   Report ID (3)
    0x09, 0x01,       //   Usage (Pointer)
    0xA1, 0x00,       //   Collection (Physical)
    0x05, 0x09,       //     Usage Page (Button)
    0x19, 0x01,       //     Usage Minimum (0x01)
    0x29, 0x03,       //     Usage Maximum (0x03)
    0x15, 0x00,       //     Logical Minimum (0)
    0x25, 0x01,       //     Logical Maximum (1)
    0x95, 0x03,       //     Report Count (3)
    0x75, 0x01,       //     Report Size (1)
    0x81, 0x02,       //     Input (Data,Var,Abs,No Wrap,Linear,...)
    0x95, 0x01,       //     Report Count (1)
    0x75, 0x05,       //     Report Size (5)
    0x81, 0x03,       //     Input (Const,Var,Abs,No Wrap,Linear,...)
    0x05, 0x01,       //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,       //     Usage (X)
    0x09, 0x31,       //     Usage (Y)
    0x15, 0x81,       //     Logical Minimum (-127)
    0x25, 0x7F,       //     Logical Maximum (127)
    0x75, 0x08,       //     Report Size (8)
    0x95, 0x02,       //     Report Count (2)
    0x81, 0x06,       //     Input (Data,Var,Rel,No Wrap,Linear,...)
    0xC0,             //   End Collection
    0xC0,             // End Collection
};

enum {
    HIDS_INPUT = 0x01,
    HIDS_OUTPUT = 0x02,
    HIDS_FEATURE = 0x03,
};

static ssize_t read_info(struct bt_conn *conn,
              const struct bt_gatt_attr *attr, void *buf,
              uint16_t len, uint16_t offset)
{
    console("hog: read_info");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
                 sizeof(struct hids_info));
}

static ssize_t read_report_map(struct bt_conn *conn,
                   const struct bt_gatt_attr *attr, void *buf,
                   uint16_t len, uint16_t offset)
{
    console("hog: read_report_map");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, report_map,
                 sizeof(report_map));
}

static ssize_t read_report(struct bt_conn *conn,
                 const struct bt_gatt_attr *attr, void *buf,
                 uint16_t len, uint16_t offset)
{
    console("hog: read_report");
	
	static uint8_t report_data[] = {
		0x00,	  // Modifer Code
		0x00, 	  // Key Code
	};
	
    return bt_gatt_attr_read(conn, attr, buf, len, offset, report_data, sizeof(report_data));
}

static ssize_t read_keyboard_report_reference(struct bt_conn *conn,
               const struct bt_gatt_attr *attr, void *buf,
               uint16_t len, uint16_t offset)
{
	static struct hids_report_reference report_reference = {
		.id = 0x01,
		.type = HIDS_INPUT,
	};
    
	console("hog: read_keyboard_report_reference");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &report_reference, sizeof(report_reference));
}

static ssize_t read_consumer_report_reference(struct bt_conn *conn,
               const struct bt_gatt_attr *attr, void *buf,
               uint16_t len, uint16_t offset)
{
	static struct hids_report_reference report_reference = {
		.id = 0x02,
		.type = HIDS_INPUT,
	};
    
	console("hog: read_consumer_report_reference");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &report_reference, sizeof(report_reference));
}

static ssize_t read_mouse_report_reference(struct bt_conn *conn,
               const struct bt_gatt_attr *attr, void *buf,
               uint16_t len, uint16_t offset)
{
	static struct hids_report_reference report_reference = {
		.id = 0x03,
		.type = HIDS_INPUT,
	};
    
	console("hog: read_mouse_report_reference");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &report_reference, sizeof(report_reference));
}

static ssize_t read_protocol_mode(struct bt_conn *conn,
                 const struct bt_gatt_attr *attr, void *buf,
                 uint16_t len, uint16_t offset)
{
    console("hog: read_protocol_mode");
	
	static uint8_t report_data[] = {
		0x01,	  // input report mode
	};
	
    return bt_gatt_attr_read(conn, attr, buf, len, offset, report_data, sizeof(report_data));
}

static ssize_t write_protocol_mode(struct bt_conn *conn,
                const struct bt_gatt_attr *attr,
                const void *buf, uint16_t len, uint16_t offset,
                uint8_t flags)
{
    console("hog: write_protocol_mode");

    uint8_t *value = attr->user_data;
	
	char snum[10];
	
	sprintf(snum, "%d", value[0]);
	console(snum);

    return len;
}

static void input_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
volatile bool notify_enable;

	//ARG_UNUSED(attr);
	notify_enable = (value == BT_GATT_CCC_NOTIFY);
    sprintf(logBuffer, "hog: input_ccc_changed: %s", notify_enable ? "enabled" : "disabled");
	console(logBuffer);
}

static ssize_t write_ctrl_point(struct bt_conn *conn,
                const struct bt_gatt_attr *attr,
                const void *buf, uint16_t len, uint16_t offset,
                uint8_t flags)
{
    uint8_t *value = attr->user_data;

    console("hog: write_ctrl_point");
    
    if (offset + len > sizeof(ctrl_point)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);

    return len;
}

/* HID Service Declaration */
BT_GATT_SERVICE_DEFINE(hog_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_HIDS),

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ,
		read_report, NULL, NULL),
		
		BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF,
			BT_GATT_PERM_READ,
			read_keyboard_report_reference, NULL, NULL),
		
		BT_GATT_CCC(input_ccc_changed,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ,
		read_report, NULL, NULL),
		
		BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF,
			BT_GATT_PERM_READ,
			read_consumer_report_reference, NULL, NULL),
		
		BT_GATT_CCC(input_ccc_changed,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ,
		read_report, NULL, NULL),
		
		BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF,
			BT_GATT_PERM_READ,
			read_mouse_report_reference, NULL, NULL),
		
		BT_GATT_CCC(input_ccc_changed,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT_MAP,
		BT_GATT_CHRC_READ, BT_GATT_PERM_READ,
		read_report_map, NULL, NULL),

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_PROTOCOL_MODE,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ,
		read_protocol_mode, write_protocol_mode, NULL),

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_INFO,
		BT_GATT_CHRC_READ, BT_GATT_PERM_READ,
		read_info, NULL, &info),

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_CTRL_POINT,
		BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_WRITE,
        NULL, write_ctrl_point, &ctrl_point),
);

void sendReport(uint8_t *buffer)
{
	int8_t result=0;
	uint8_t keyboard_report_data[2];
	uint8_t consumer_report_data[2];
	uint8_t mouse_report_data[3];
	uint8_t reportId = buffer[1];
		
	if(isConnected == 0) {
		sprintf(logBuffer, "Abort: Peer not connected.");
		console(logBuffer);
		startAdvertising();
		return;
	}
	
	switch(reportId) {

	case 1:
		//keyboard report data
		keyboard_report_data[0] = buffer[2]; // Modifer Code
		keyboard_report_data[1] = buffer[3]; // Key Code
		
		result = bt_gatt_notify(NULL, &hog_svc.attrs[2], keyboard_report_data, sizeof(keyboard_report_data));
		sprintf(logBuffer, "***Sent Keyboard Report1: x%02x, x%02x result: %d", keyboard_report_data[0], keyboard_report_data[1], result);
		console(logBuffer);
		
		break;
	case 2:
		//consumer report data
		consumer_report_data[0] = buffer[2]; // lowbyte Key Code
		consumer_report_data[1] = buffer[3]; // highbyte Key Code

		result = bt_gatt_notify(NULL, &hog_svc.attrs[5], consumer_report_data, sizeof(consumer_report_data));
		sprintf(logBuffer, "***Sent Consumer Report2: x%02x, x%02x result: %d", consumer_report_data[0], consumer_report_data[1], result);
		console(logBuffer);
		
		break;
	case 3:
		//mouse report data
		mouse_report_data[0] = buffer[2]; // Modifer Code
		mouse_report_data[1] = buffer[3]; // Key Code
		mouse_report_data[2] = buffer[4]; // Key Code

 		result = bt_gatt_notify(NULL, &hog_svc.attrs[9], mouse_report_data, sizeof(mouse_report_data));
		sprintf(logBuffer, "***Sent Mouse Report3: x%02x, x%02x, x%02x, result: %d", mouse_report_data[0], mouse_report_data[1], mouse_report_data[2], result);
		console(logBuffer);
		
		break;
	default :
		sprintf(logBuffer, "Invalid reportId: %d", reportId);
		console(logBuffer);
	}
}

void hog_init(void)
{
    console("hog: hog_init");
}
