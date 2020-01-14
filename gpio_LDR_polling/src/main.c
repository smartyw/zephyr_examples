/**
 * GPIO polling. Reads a digital value from an LDR connected to PIN 0 every n ms in a loop
 * Connected per http://www.getmicros.net/microbit-ldr-example.php
 *
 * Bright      : 0
 * Dark        : 1
 *
 **/
#include <gpio.h>
#include <board.h>
#include <device.h>

// GPIO
struct device *gpio_dev;
#define PORT SW0_GPIO_CONTROLLER
#define PIN_0 EXT_P0_GPIO_PIN
#define LED0 EXT_P0_GPIO_PIN

#define DELAY_MS 1000

void poll(void) 
{
	while (1) {
		u32_t val = 0;
		gpio_pin_read(gpio_dev, PIN_0, &val);
		printk("%d\n",val);
		k_sleep(K_MSEC(DELAY_MS));
	}
}

void configureGpio(void)
{
	gpio_dev = device_get_binding(PORT);
	if (!gpio_dev)
	{
		printk("error - no GPIO device\n");
		return;
	}
	gpio_pin_configure(gpio_dev, PIN_0, GPIO_DIR_IN);
}

void main(void)
{
	printk("GPIO input polling V1.0\n");
	configureGpio();
	poll();
}
