cmake_minimum_required(VERSION 3.13)

# รับค่า version จาก workflow (ถ้าไม่มีใช้ dev)
if(NOT DEFINED NUKE_VERSION)
    set(NUKE_VERSION "dev")
endif()

# หา Pico SDK
if(NOT DEFINED PICO_SDK_PATH)
    set(PICO_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../pico-sdk")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(flash_nuke C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(flash_nuke
    src/flash_nuke.c
)

# ⭐ หัวใจของโปรเจกต์นี้ — รันจาก RAM เพื่อให้ลบ Flash ตัวเองได้ปลอดภัย
pico_set_binary_type(flash_nuke copy_to_ram)

target_compile_definitions(flash_nuke PRIVATE
    NUKE_VERSION="${NUKE_VERSION}"
)

target_link_libraries(flash_nuke
    pico_stdlib
    pico_bootrom
    hardware_flash
    hardware_sync
    hardware_gpio
)

# สร้างไฟล์ .uf2, .elf, .bin, .hex
pico_add_extra_outputs(flash_nuke)
