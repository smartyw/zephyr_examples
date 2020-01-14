/**
 * GPIO interrupts. Triggered on change of LDR level connected to PIN 0
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
static struct gpio_callback gpio_trigger_cb;

#define PORT SW0_GPIO_CONTROLLER
#define PIN_0 EXT_P0_GPIO_PIN
#define LED0 EXT_P0_GPIO_PIN

void triggered(struct device *gpiob, struct gpio_callback *cb, u32_t pins)
{
	u32_t val = 0;
	gpio_pin_read(gpio_dev, PIN_0, &val);
	if (val == 0) {
	    printk("%d LIGHT\n", k_cycle_get_32());
	} else {
		printk("%d DARK\n", k_cycle_get_32());
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
	printk("pin P0 triggering when low (bright)\n");

	// GPIO_INT: GPIO pin to trigger interrupt
	// GPIO_INT_EDGE: Do Edge trigger i.e. only when value changes as opposed to coninuously after it has changed
	// GPIO_INT_ACTIVE_LOW: GPIO pin trigger on level low or falling edge
    gpio_pin_configure(gpio_dev, PIN_0, GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW  );

	gpio_init_callback(&gpio_trigger_cb, triggered, BIT(PIN_0));
	gpio_add_callback(gpio_dev, &gpio_trigger_cb);
	gpio_pin_enable_callback(gpio_dev, PIN_0);

}

void main(void)
{
	printk("GPIO interrupt triggers V1.0\n");
	configureGpio();
}
