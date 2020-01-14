/**
 * GPIO on / off. Switch an LED on / off by pressing buttons.
 * 
 **/
#include <gpio.h>
#include <board.h>
#include <device.h>

// GPIO
struct device *gpio_dev;

// GPIO for the buttons
#define PIN_A SW0_GPIO_PIN
#define PIN_B SW1_GPIO_PIN
#define PORT SW0_GPIO_CONTROLLER
#define EDGE (GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW | GPIO_INT_DEBOUNCE)

static struct gpio_callback gpio_btnA_cb;
static struct gpio_callback gpio_btnB_cb;

// GPIO for the connected LED
// See board.h in zephyr/boards/arm/bbc_microbit
#define LED0 EXT_P0_GPIO_PIN

// for use with k_work_submit which we use to handle button presses in a background thread to avoid holding onto an IRQ for too long
static struct k_work buttonA_work;
static struct k_work buttonB_work;

void set_led(int state) {
	printk("set_led %d\n",state);
	gpio_pin_write(gpio_dev, LED0, state);
}

void buttonA_work_handler(struct k_work *work)
{
	printk("Button A work handler\n");
	set_led(1);
}

void buttonB_work_handler(struct k_work *work)
{
	printk("Button B work handler\n");
	set_led(0);
}

void button_A_pressed(struct device *gpiob, struct gpio_callback *cb,
											u32_t pins)
{
	printk("Button A pressed at %d\n", k_cycle_get_32());
	k_work_submit(&buttonA_work);
}

void button_B_pressed(struct device *gpiob, struct gpio_callback *cb,
											u32_t pins)
{
	printk("Button B pressed at %d\n", k_cycle_get_32());
	k_work_submit(&buttonB_work);
}

// Connected LED

void led_init()
{
	gpio_pin_configure(gpio_dev, LED0, GPIO_DIR_OUT);
}


// -------------------------------------------------------------------------------------------------------
// Buttons
// -------

void configureButtons(void)
{
	printk("Press button A or button B\n");
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
	printk("GPIO on off V1.0\n");

	gpio_dev = device_get_binding(PORT);

	configureButtons();
	led_init();
}
