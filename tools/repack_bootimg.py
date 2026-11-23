#!/usr/bin/env python3
import sys, struct, os

def repack(kernel_path, ramdisk_path, out_path, cmdline="buildvariant=userdebug", page_size=2048, base=0x10000000):
    with open(kernel_path, "rb") as f:
        kernel = f.read()
    with open(ramdisk_path, "rb") as f:
        ramdisk = f.read()

    kernel_addr = base + 0x00008000
    ramdisk_addr = base + 0x01000000
    tags_addr = base + 0x00000100

    k_size = len(kernel)
    r_size = len(ramdisk)

    header = struct.pack(
        '<8s10I16s512s32s1024s',
        b'ANDROID!',
        k_size, kernel_addr,
        r_size, ramdisk_addr,
        0, 0x10f00000,
        tags_addr, page_size,
        0, 0,
        b'',
        cmdline.encode('ascii')[:512],
        b'\x00' * 32,
        b'\x00' * 1024
    )

    header_padded = header.ljust(page_size, b'\x00')
    k_pad = ((k_size + page_size - 1) // page_size) * page_size
    kernel_padded = kernel.ljust(k_pad, b'\x00')
    r_pad = ((r_size + page_size - 1) // page_size) * page_size
    ramdisk_padded = ramdisk.ljust(r_pad, b'\x00')

    with open(out_path, "wb") as f:
        f.write(header_padded)
        f.write(kernel_padded)
        f.write(ramdisk_padded)
    print(f"SUCCESS: Repacked {out_path} ({os.path.getsize(out_path)} bytes)")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: repack_bootimg.py <bzImage> <ramdisk> <output_boot.img>")
        sys.exit(1)
    repack(sys.argv[1], sys.argv[2], sys.argv[3])
