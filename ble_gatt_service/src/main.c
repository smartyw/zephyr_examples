/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>

#include <services/random_number_service.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

struct bt_conn *default_conn;

// advertising data
static const struct bt_data ad[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
		BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};


static void connected(struct bt_conn *conn, u8_t err)
{
	if (!err)
	{
		default_conn = bt_conn_ref(conn);
		set_connection(conn);
	}
	else
	{
		printk("Connection failed (err %u)\n", err);
	}
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
	printk("Disconnected (reason %u)\n", reason);

	stop_generating_random_numbers();

	if (default_conn)
	{
		bt_conn_unref(default_conn);
		default_conn = NULL;
	}
}

static struct bt_conn_cb conn_callbacks = {
		.connected = connected,
		.disconnected = disconnected,
};

static void bt_ready(int err)
{
	if (err)
	{
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	random_number_service_init();

	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err)
	{
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

void main(void)
{

	printk("starting\n");

	int err;

	err = bt_enable(bt_ready);
	if (err)
	{
		return;
	}

	// bt_conn_cb_register(&conn_callbacks);
}
