/*
 * flash_nuke.c — ล้าง Flash ทั้งชิป RP2350 128MB
 * ใช้กับ Tenstar RP2350 USB-A
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"

#ifndef NUKE_VERSION
#define NUKE_VERSION "dev"
#endif

#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (128u * 1024u * 1024u)   /* 128 MB fallback */
#endif

#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN 22
#endif

/* ------------------------------------------------------------------
 * ลบ Flash ทั้งชิป — ต้องรันจาก RAM
 * ------------------------------------------------------------------ */
static void __no_inline_not_in_flash_func(nuke_flash_and_reboot)(void)
{
    uint32_t ints = save_and_disable_interrupts();

    /* ลบทั้งชิป — 128 MB ใช้เวลา ~40-90 วินาที */
    flash_range_erase(0, PICO_FLASH_SIZE_BYTES);

    (void)ints;

    reset_usb_boot(0, 0);
    while (1) tight_loop_contents();
}

/* ------------------------------------------------------------------
 * กระพริบ LED (ใช้ WS2812 pin 22 ก็ใช้ digitalWrite ได้ชั่วคราว)
 * ------------------------------------------------------------------ */
static void blink(int times, int ms)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    for (int i = 0; i < times; i++) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1); sleep_ms(ms);
        gpio_put(PICO_DEFAULT_LED_PIN, 0); sleep_ms(ms);
    }
}

int main(void)
{
    stdio_init_all();
    sleep_ms(1500);

    uint32_t flash_mb = PICO_FLASH_SIZE_BYTES / (1024u * 1024u);

    printf("\n");
    printf("==========================================\n");
    printf("  FLASH NUKE  %s\n", NUKE_VERSION);
    printf("  Target: RP2350 (Tenstar USB-A)\n");
    printf("  Flash size: %u MB\n", flash_mb);
    printf("  ⚠️  ใช้เวลา ~40-90 วินาที ห้ามถอด USB!\n");
    printf("  เริ่มลบใน 3 วินาที...\n");
    printf("==========================================\n");

    /* เตือน 3 ครั้ง */
    blink(3, 150);

    sleep_ms(3000);

    printf("🧹 กำลังลบ Flash ทั้งหมด (%u MB)...\n", flash_mb);
    printf("   LED จะค้าง — รอจนกว่า USB bootloader จะกลับมา\n");
    fflush(stdout);

    nuke_flash_and_reboot();
    return 0;
}
