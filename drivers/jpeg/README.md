# JPEG SW Library

Prebuilt static library for VeriSilicon Hantro VC9000E JPEG encoder
SW (quantization tables, Huffman tables, JPEG header).

## Current Library Version: 1.0.0

Source repository: `alif-vc9000e-jpeg-lib` (private)

## Directory Structure

```
jpeg/
├── CMakeLists.txt      # Auto-selects .a based on ZEPHYR_TOOLCHAIN_VARIANT
├── Kconfig             # CONFIG_USE_ALIF_JPEG_SW_LIB toggle
├── Lib/
│   ├── libjpeg_hantro_sw_gcc.a
│   ├── libjpeg_hantro_sw_armclang.a
│   └── libjpeg_hantro_sw_llvm.a
├── inc/
│   └── jpeg_hantro_vc9000e_sw.h
└── README.md
```

## Prebuilt Libraries

All libraries target **Cortex-M55, Thumb, hard-float**.

| Toolchain | Library File | Compiler Used |
|---|---|---|
| GCC | `libjpeg_hantro_sw_gcc.a` | arm-zephyr-eabi-gcc (Zephyr SDK 0.16.5) |
| ARMCLANG | `libjpeg_hantro_sw_armclang.a` | Arm Compiler 6.23 |
| LLVM | `libjpeg_hantro_sw_llvm.a` | LLVM Embedded Toolchain for Arm 17.0.1 |

## Toolchain Selection

The correct `.a` is **automatically selected** based on `ZEPHYR_TOOLCHAIN_VARIANT`:

| `ZEPHYR_TOOLCHAIN_VARIANT` | Library linked |
|---|---|
| `zephyr` (default) | `libjpeg_hantro_sw_gcc.a` |
| `armclang` | `libjpeg_hantro_sw_armclang.a` |
| `llvm` | `libjpeg_hantro_sw_llvm.a` |
| any other | `libjpeg_hantro_sw_gcc.a` (fallback) |

No manual configuration needed — it follows your Zephyr build toolchain.

## Verifying Library Version

```bash
strings Lib/libjpeg_hantro_sw_gcc.a | grep "alif-vc9000e-jpeg-lib"
# Output: alif-vc9000e-jpeg-lib v1.0.0
```
