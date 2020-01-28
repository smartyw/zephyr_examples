
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <misc/printk.h>
#include <misc/byteorder.h>
#include <zephyr.h>
#include <stdio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <services/random_number_service.h>

u32_t random_number;
// in ms and therefore value is 100 x that of the corresponding characteristic
u32_t rnd_frequency;
struct k_delayed_work random_number_timer;
u8_t random_number_subscribed;
u8_t is_generating;

static struct bt_gatt_ccc_cfg rnd_ccc_cfg[BT_GATT_CCC_MAX] = {};
struct bt_conn *the_conn;


// A completely made up example GATT service with two characteristics to play with
// The first is a characteristic whose value is a random number, 32-bit number.
// It can be read and supports notifications but cannot be written to.
// The random number value changes at a frequency determined by the second characteristic. 
// 
// The second characteristic contains a 16-bit integer which when multipled by 100 represents
// the frequency with which new random numbers are generated for the first characteristic, 
// measured in milliseconds. 
//
// Characteristic values are in Little Endian format

// UUIDs were generated using uuidgen on a Mac
#define BT_UUID_RND_SERVICE   BT_UUID_DECLARE_128( 0x74,0xC1,0xCA,0xC8,0x47,0x72,0x46,0x75,0x96,0xF7,0x8B,0x77,0x4B,0x97,0x10,0xFC)
#define BT_UUID_RANDOM_NUMBER BT_UUID_DECLARE_128( 0x5A,0x6A,0xBE,0x2A,0x8D,0xA1,0x4E,0x78,0xA5,0xA5,0xCA,0x7A,0x4B,0xEC,0x72,0x09 )
#define BT_UUID_RND_FREQUENCY BT_UUID_DECLARE_128( 0x8A,0x42,0x53,0x16,0x1D,0x5D,0x4A,0xEC,0xB9,0xB4,0x5F,0xDE,0xD9,0xB0,0x3E,0x99 )


/* Random Number Service Declaration */
static struct bt_gatt_attr attrs[] = {
		BT_GATT_PRIMARY_SERVICE(BT_UUID_RND_SERVICE),
		//BT_GATT_CHARACTERISTIC: uuid, properties, access perms, read callback, write callback, value
		BT_GATT_CHARACTERISTIC(BT_UUID_RANDOM_NUMBER, BT_GATT_CHRC_READ|BT_GATT_CHRC_NOTIFY,BT_GATT_PERM_READ, NULL, NULL, NULL),
		// BT_GATT_CCC: initial config, changed callback
		BT_GATT_CCC(rnd_ccc_cfg, rnd_ccc_cfg_changed),

		BT_GATT_CHARACTERISTIC(BT_UUID_RND_FREQUENCY, BT_GATT_CHRC_WRITE_WITHOUT_RESP,BT_GATT_PERM_WRITE, NULL, write_rnd_frequency, NULL),
};

static struct bt_gatt_service random_number_svc = BT_GATT_SERVICE(attrs);

void random_number_service_init()
{
	bt_gatt_service_register(&random_number_svc);
	rnd_frequency = 10; // once a second default
	random_number = 0;
}

ssize_t write_rnd_frequency(struct bt_conn *conn,const struct bt_gatt_attr *attr,const void *buf, u16_t len, u16_t offset,u8_t flags)
{
	printk("write_rnd_frequency\n");

	const u16_t *new_value = buf;

	if (len != 2)
	{
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	rnd_frequency = new_value[0] + (new_value[1] ^ 2);

	printk("rnd_frequency: (%d)\n", rnd_frequency);

	return len;
}

void set_connection(struct bt_conn *conn) {
	the_conn = conn;
}

void rnd_ccc_cfg_changed(const struct bt_gatt_attr *attr, u16_t value)
{
	random_number_subscribed = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
	printk("random_number_subscribed set to %d\n", random_number_subscribed);
	if (random_number_subscribed == 1) {
		start_generating_random_numbers();
	}
}

void notify_random_number(u32_t random_number)
{
    unsigned char random_number_bytes[4] = {0, 0, 0, 0};

	// NB: little endian
	random_number_bytes[3] = (random_number >> 24);
	random_number_bytes[2] = (random_number >> 16);
	random_number_bytes[1] = (random_number >> 8);
	random_number_bytes[0] = random_number;

	bt_gatt_notify(the_conn, &attrs[1],&random_number_bytes, 4);
}

u8_t get_random_number_subscribed()
{
	return random_number_subscribed;
}

void generate_random_number()
{
	random_number = sys_rand32_get();
	notify_random_number(random_number);

	// and again
	if (is_generating == 1) {
		k_delayed_work_submit(&random_number_timer, K_MSEC(rnd_frequency));
	}
}

void start_generating_random_numbers()
{
	is_generating = 1;
    k_delayed_work_init(&random_number_timer, generate_random_number);
	k_delayed_work_submit(&random_number_timer, K_MSEC(rnd_frequency));
}

void stop_generating_random_numbers()
{
	printk("stopping generating random numbers\n");
	is_generating = 0;
}

