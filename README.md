# flash-nuke-rp2350

เฟิร์มแวร์จิ๋วสำหรับ **ล้าง Flash ทั้งชิป RP2350** แล้ว reboot เข้า USB bootloader อัตโนมัติ

ใช้เมื่อ:
- เฟิร์มแวร์ที่ลงอยู่ทำงานเพี้ยนเพราะ config เก่าค้างใน data region
- อยากเริ่มต้นใหม่แบบสะอาด 100% ก่อนแฟลชเฟิร์มแวร์อื่น
- กู้บอร์ดที่ bootloop จากค่า config เสีย

## ดาวน์โหลด

ไปที่ [Releases](../../releases) เลือกไฟล์ `flash_nuke-*.uf2` ล่าสุด  
หรือโหลดจาก Actions artifact

## วิธีใช้

### แบบลากไฟล์ (ง่ายสุด)
1. กดปุ่ม **BOOTSEL** ค้างไว้ แล้วเสียบ USB
2. เครื่องจะเห็นไดรฟ์ชื่อ `RP2350`
3. ลาก `flash_nuke.uf2` ไปวางในไดรฟ์
4. รอ 3-5 วินาที — บอร์ดจะ reboot ตัวเองเข้า bootloader mode อีกครั้ง
5. ลากเฟิร์มแวร์จริงที่ต้องการลงไป

### แบบ picotool
```bash
picotool load -f -x flash_nuke.uf2
# รอ bootloader กลับมา
picotool info
# แฟลชเฟิร์มแวร์จริง
picotool load -f -x your_firmware.uf2
```

## Build เอง

```bash
git clone https://github.com/<your-user>/flash-nuke-rp2350.git
cd flash-nuke-rp2350
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk && git submodule update --init && cd ..

cmake -S . -B build -DPICO_BOARD=pico2 -DPICO_PLATFORM=rp2350
cmake --build build -j$(nproc)
# → build/flash_nuke.uf2
```

## License

MIT
