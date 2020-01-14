/*
 * Note that the Zephyr NVS API is built on top of the "raw" flash memory APIs.
 * When initialising, it checks to see if there is any data in flash which was not created
 * using NVS. If so, initialisatio fails. In this case, flash must be cleared. In the case
 * of micro:bit, this is slightly complicated. The tool to use is nrfjprog --family nRF51 --eraseall
 * but micro:bit does not normally support JLink. So you have to replace the DAPLink firmware with
 * JLink per https://www.segger.com/products/debug-probes/j-link/models/other-j-links/bbc-microbit-j-link-upgrade/ 
 * After clearing flash, you must reinstall the DAPlink firmware so that west and the tools it
 * controls will work with micro:bit, since it is expected that it is using DAPLink.
 * See https://support.microbit.org/support/solutions/articles/19000019131-upgrade-the-firmware-on-the-micro-bit
 *
 */
#include <board.h>
#include <gpio.h>
#include <flash.h>
#include <nvs/nvs.h>

// NVS file system
static struct nvs_fs fs;
static struct flash_pages_info info;
#define DATA_ID 1
u32_t value = 0U;

// GPIO
struct device *gpio_dev;

// GPIO for the buttons
#define PIN_A SW0_GPIO_PIN
#define PIN_B SW1_GPIO_PIN
#define PORT SW0_GPIO_CONTROLLER
#define EDGE (GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW | GPIO_INT_DEBOUNCE)

static struct gpio_callback gpio_btnA_cb;
static struct gpio_callback gpio_btnB_cb;

#define BUTTON_DEBOUNCE_DELAY_MS 250
static u32_t time, last_time;

// for use with k_work_submit which we use to handle button presses in a background thread to avoid holding onto an IRQ for too long
static struct k_work buttonA_work;
static struct k_work buttonB_work;

#define DATA_ID 1

bool debounce() {
	bool result = false;
	time = k_uptime_get_32();
	if (time < last_time + BUTTON_DEBOUNCE_DELAY_MS) {
		result = true;
	}
	last_time = time;
    return result;
}

void buttonA_work_handler(struct k_work *work)
{
	if (debounce()) {
		return;
	}
    value = sys_rand32_get();
	printk("Storing random number %d\n",value);
	int bytes_written = nvs_write(&fs, DATA_ID, &value, sizeof(value));
	if (bytes_written < 0) {
		printk("nvs_write error %d\n",bytes_written);
	} else {
		printk("nvs_write OK [%d]\n",bytes_written);
	}
}

void buttonB_work_handler(struct k_work *work)
{
	if (debounce()) {
		return;
	}
    int err = nvs_read(&fs, DATA_ID, &value, sizeof(value));
	if (err > 0) { 
		printk("Value: %d\n",value);
	} else   {
		printk("Data not found %d\n",err);
	}
}

void button_A_pressed(struct device *gpiob, struct gpio_callback *cb,
											u32_t pins)
{
	// printk("Button A pressed at %d\n", k_cycle_get_32());
	k_work_submit(&buttonA_work);
}

void button_B_pressed(struct device *gpiob, struct gpio_callback *cb,
											u32_t pins)
{
	// printk("Button B pressed at %d\n", k_cycle_get_32());
	k_work_submit(&buttonB_work);
}

// NVS flash

void initialiseNvs(void) {
    int err = 0;
	fs.offset = DT_FLASH_AREA_STORAGE_OFFSET;
	err = flash_get_page_info_by_offs(device_get_binding(DT_FLASH_DEV_NAME),
					 fs.offset, &info);
	if (err) {
		printk("flash_get_page_info_by_offs failed (err %d)\n", err);
		return;
	}
	fs.sector_size = info.size;
	fs.sector_count = 3U;

	err = nvs_init(&fs, DT_FLASH_DEV_NAME);
	if (err) {
		printk("Flash Init failed: %d\n",err);
	} else {
		printk("Flash initialised OK\n");
	}

}

// -------------------------------------------------------------------------------------------------------
// Buttons
// -------

void configureButtons(void)
{
	if (!gpio_dev)
	{
		printk("error - no GPIO device\n");
		return;
	}

	// Button A
	k_work_init(&buttonA_work, buttonA_work_handler);
	gpio_pin_configure(gpio_dev, PIN_A, GPIO_DIR_IN | GPIO_INT | EDGE);
	gpio_init_callback(&gpio_btnA_cb, button_A_pressed, BIT(PIN_A));
	gpio_add_callback(gpio_dev, &gpio_btnA_cb);
	gpio_pin_enable_callback(gpio_dev, PIN_A);

	// Button B
	k_work_init(&buttonB_work, buttonB_work_handler);
	gpio_pin_configure(gpio_dev, PIN_B, GPIO_DIR_IN | GPIO_INT | EDGE);
	gpio_init_callback(&gpio_btnB_cb, button_B_pressed, BIT(PIN_B));
	gpio_add_callback(gpio_dev, &gpio_btnB_cb);
	gpio_pin_enable_callback(gpio_dev, PIN_B);
}

void main(void)
{
	printk("NVS example V1.0\n");

	gpio_dev = device_get_binding(PORT);
	configureButtons();

	initialiseNvs();

}
