cmake_minimum_required(VERSION 3.13)

# ============================================================
# Version (รับจาก workflow, ถ้าไม่มีใช้ "dev")
# ============================================================
if(NOT DEFINED NUKE_VERSION)
    set(NUKE_VERSION "dev")
endif()

# ============================================================
# Pico SDK path
# ============================================================
if(NOT DEFINED PICO_SDK_PATH)
    set(PICO_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../pico-sdk")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# ============================================================
# ใช้ pico2 เป็นฐาน (compatible กับ RP2350 ทุกบอร์ด)
# ============================================================
set(PICO_BOARD pico2)

# ============================================================
# Project
# ============================================================
project(flash_nuke C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# ============================================================
# Target
# ============================================================
add_executable(flash_nuke
    src/flash_nuke.c
)

# ============================================================
# Compile definitions
# ============================================================
target_compile_definitions(flash_nuke PRIVATE
    NUKE_VERSION="${NUKE_VERSION}"
    PICO_FLASH_SIZE_BYTES=134217728          # 128 MB — Tenstar RP2350 USB-A
)

# ============================================================
# ⭐ รันจาก RAM เท่านั้น — จำเป็นสำหรับการ erase flash ตัวเอง
# ============================================================
pico_set_binary_type(flash_nuke copy_to_ram)

# ============================================================
# Libraries
# ============================================================
target_link_libraries(flash_nuke
    pico_stdlib
    pico_bootrom
    hardware_flash
    hardware_sync
    hardware_gpio
)

# ============================================================
# สร้างไฟล์ .uf2, .elf, .bin, .hex
# ============================================================
pico_add_extra_outputs(flash_nuke)

# ============================================================
# (Optional) แสดงขนาด firmware หลัง build — แบบปลอดภัย
#   ใช้เฉพาะเมื่อ find_program เจอ arm-none-eabi-size เท่านั้น
# ============================================================
find_program(CMAKE_SIZE arm-none-eabi-size)

if(CMAKE_SIZE)
    add_custom_command(TARGET flash_nuke POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:flash_nuke>
        COMMENT "Firmware size:"
    )
endif()
