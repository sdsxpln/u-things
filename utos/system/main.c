/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "config.h"
#include "common.h"
#include "uart.h"
#include "shell.h"
#include "log.h"
#include "gpio.h"
#include "timer.h"
#include "hw_config.h"

extern unsigned char  Load$$ER_IROM1$$Base;
extern unsigned char Image$$ER_IROM1$$Base;
extern unsigned char Image$$ER_IROM1$$Length;

extern unsigned char  Load$$RW_IRAM1$$Base;
extern unsigned char Image$$RW_IRAM1$$Base;
extern unsigned char Image$$RW_IRAM1$$Length;

uint32_t flash_base = FLASH_BASE, flash_size = FLASH_SIZE;
uint32_t ram_base = SRAM_BASE, ram_size = SRAM_SIZE;

uint32_t flash_load_base, flash_image_base, flash_image_size;
uint32_t ram_load_base, ram_image_base, ram_image_size;

char sys_banner[] = {"utos system buildtime [" __TIME__ " " __DATE__ "] " "rev " XXXX_REV};

/*
 * main: initialize and start the system
 */
int main (void)
{
	uart_init();
	timer_init();
	
	/* ����һ���ַ��� */
	PRINT_EMG("%s\n", sys_banner);

	flash_load_base  = (uint32_t)&Load$$ER_IROM1$$Base;
	flash_image_base = (uint32_t)&Image$$ER_IROM1$$Base;
	flash_image_size = (uint32_t)&Image$$ER_IROM1$$Length;

	ram_load_base  = (uint32_t)&Load$$RW_IRAM1$$Base;
  ram_image_base = (uint32_t)&Image$$RW_IRAM1$$Base;
  ram_image_size = (uint32_t)&Image$$RW_IRAM1$$Length;
	
	PRINT_EMG("flash memory [0x%08x, 0x%08x]\n", flash_base, flash_base + flash_size);
	PRINT_EMG("ram   memory [0x%08x, 0x%08x]\n", ram_base, ram_base + ram_size);

	DUMP_VAR4(flash_load_base);
	DUMP_VAR4(flash_image_base);
	DUMP_VAR4(flash_image_size);

	DUMP_VAR4(ram_load_base);
	DUMP_VAR4(ram_image_base);
	DUMP_VAR4(ram_image_size);

 /* micro usb��˵���� 
		GND-��ɫ : GND
		VBUS-��ɫ: VCC 3.3
		DM-��ɫ  : GPIOA.5
		DP-��ɫ  : GPIOA.6
 */
	gpio_init(GROUPA, 5, GPIO_Mode_Out_PP);	/* SPI SCK  */
	gpio_init(GROUPA, 6, GPIO_Mode_Out_PP); /* SPI MISO */
	gpio_init(GROUPA, 7, GPIO_Mode_Out_PP);/*  SPI MOSI */
	
	gpio_write(GROUPA, 5, 1);
	gpio_write(GROUPA, 6, 0);
	gpio_write(GROUPA, 7, 1);

	gpio_init(GROUPB, 1, GPIO_Mode_Out_PP);
	gpio_write(GROUPB, 1, 0);
	USB_Config();
	while(1) {
		gpio_write(GROUPB, 1, 0);
		mdelay(1000);
		gpio_write(GROUPB, 1, 1);
		mdelay(1000);
	}
	
#if 0	
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	
  // initialize peripherals here
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	main_thread_id  = osThreadCreate(osThread(thread_main),  NULL);
	blink_thread_id = osThreadCreate(osThread(thread_blink), NULL);
	DUMP_VAR4(main_thread_id);
	DUMP_VAR4(blink_thread_id);
	
	shell_init();

	osKernelStart ();                         // start thread execution 
#endif
}