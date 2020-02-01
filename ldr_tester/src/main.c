/**
 * Flashes the central LED of a microbit at varying intervals. Used to test LDR use in anotehr project.
 * 
 **/
#include <board.h>
#include <device.h>

#include <display/mb_display.h>

static struct mb_image splash = 
MB_IMAGE({ 1, 1, 1, 1, 1 },
		 { 1, 1, 1, 1, 1 },
		 { 1, 1, 1, 1, 1 },
		 { 1, 1, 1, 1, 1 },
		 { 1, 1, 1, 1, 1 });


static struct mb_image led_on = 
MB_IMAGE({ 0, 0, 0, 0, 0 },
		 { 0, 0, 0, 0, 0 },
		 { 0, 0, 1, 0, 0 },
		 { 0, 0, 0, 0, 0 },
		 { 0, 0, 0, 0, 0 });

void main(void)
{
	printk("LDR tester V1.0.1\n");
	struct mb_display *disp = mb_display_get();
	mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, K_MSEC(2000), &splash, 1);
	k_sleep(K_MSEC(3000));
	u32_t sleep_time = 14000; // 14 seconds
	s32_t delta = 0;
	while(1) {
		mb_display_image(disp, MB_DISPLAY_MODE_SINGLE, K_MSEC(250), &led_on, 1);
		printk("sleeping for %dms\n",sleep_time);
		k_sleep(K_MSEC(sleep_time));
		u32_t random_number = sys_rand32_get();
		delta = (random_number % 2000) - 1000;
		sleep_time = sleep_time + delta;
	}
}
