cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED NUKE_VERSION)
    set(NUKE_VERSION "dev")
endif()

if(NOT DEFINED PICO_SDK_PATH)
    set(PICO_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../pico-sdk")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# ⭐ ใช้บอร์ด pico2 เป็นฐาน แล้ว override ขนาด flash ทีหลัง
set(PICO_BOARD pico2)

project(flash_nuke C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(flash_nuke
    src/flash_nuke.c
)

# ⭐ บังคับขนาด Flash 128 MB
target_compile_definitions(flash_nuke PRIVATE
    NUKE_VERSION="${NUKE_VERSION}"
    PICO_FLASH_SIZE_BYTES=134217728          # 128 MB
    PICO_FLASH_SIZE_BYTES_OVERRIDE=1
)

# ⭐ สำคัญมาก — รันจาก RAM
pico_set_binary_type(flash_nuke copy_to_ram)

target_link_libraries(flash_nuke
    pico_stdlib
    pico_bootrom
    hardware_flash
    hardware_sync
    hardware_gpio
)

pico_add_extra_outputs(flash_nuke)

# แสดงขนาดหลัง build
add_custom_command(TARGET flash_nuke POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:flash_nuke>
    COMMENT "Firmware size:"
)
