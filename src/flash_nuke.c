/*
 * flash_nuke.c — ล้าง Flash ทั้งชิป RP2350 แล้ว reboot เข้า USB bootloader
 *
 * หลักการ:
 *   - โค้ดทั้งหมดถูก link ไว้ใน RAM (copy_to_ram) เพื่อให้ปลอดภัยเวลา erase flash
 *   - ใช้ flash_range_erase() ลบทั้งชิป แล้วเรียก reset_usb_boot() จาก ROM
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"

#ifndef NUKE_VERSION
#define NUKE_VERSION "dev"
#endif

#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN -1
#endif

/* ------------------------------------------------------------------
 * ฟังก์ชันนี้ต้องรันจาก RAM เท่านั้น — เรากำลังจะลบ Flash ที่มันอาศัยอยู่
 * ------------------------------------------------------------------ */
static void __no_inline_not_in_flash_func(nuke_flash_and_reboot)(void)
{
    /* ปิด interrupt ระหว่างลบ Flash (erase ใช้เวลานาน) */
    uint32_t ints = save_and_disable_interrupts();

    /* ลบ Flash ทั้งชิป — pico-sdk จะจัดการ XIP ให้เอง */
    flash_range_erase(0, PICO_FLASH_SIZE_BYTES);

    (void)ints;

    /* ROM function อยู่ใน ROM จริง — เรียกได้แม้ Flash ว่าง */
    reset_usb_boot(0, 0);

    while (1) tight_loop_contents();
}

/* ------------------------------------------------------------------ */

static void blink_led(int pin, int times)
{
    if (pin < 0) return;
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    for (int i = 0; i < times; i++) {
        gpio_put(pin, 1); sleep_ms(120);
        gpio_put(pin, 0); sleep_ms(120);
    }
}

int main(void)
{
    stdio_init_all();
    sleep_ms(1500);   /* รอ USB enumerate ให้ผู้ใช้เห็นข้อความ */

    printf("\n");
    printf("==========================================\n");
    printf("  FLASH NUKE  %s\n", NUKE_VERSION);
    printf("  Target: RP2350  |  Flash size: %u bytes\n",
           (unsigned)PICO_FLASH_SIZE_BYTES);
    printf("  กำลังลบ Flash ทั้งชิปใน 2 วินาที...\n");
    printf("==========================================\n");

    /* เตือนด้วย LED 3 ครั้ง */
    blink_led(PICO_DEFAULT_LED_PIN, 3);

    sleep_ms(2000);

    printf("🧹 Erasing...\n");
    sleep_ms(200);

    nuke_flash_and_reboot();

    return 0;   /* ไม่มีทางมาถึง */
}
