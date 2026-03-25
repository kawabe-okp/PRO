/* Custom linker script for mbed LPC1768 */
/* Moves .bss to USB_RAM to maximize heap in main RAM */

#if !defined(MBED_APP_START)
  #define MBED_APP_START 0x00000000
#endif

#if !defined(MBED_APP_SIZE)
  #define MBED_APP_SIZE 512K
#endif

#if !defined(MBED_BOOT_STACK_SIZE)
    #define MBED_BOOT_STACK_SIZE 0x400
#endif

STACK_SIZE = MBED_BOOT_STACK_SIZE;

MEMORY
{
  FLASH (rx)    : ORIGIN = MBED_APP_START, LENGTH = MBED_APP_SIZE
  RAM (rwx)     : ORIGIN = 0x100000C8, LENGTH = (32K - 0xC8 - 32)
  USB_RAM(rwx)  : ORIGIN = 0x2007C000, LENGTH = 16K
  ETH_RAM(rwx)  : ORIGIN = 0x20080000, LENGTH = 16K
}

ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        . = 0x000002FC ;
        KEEP(*(.CRPSection))
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))

    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    __etext = .;

    /* Initialized data in main RAM */
    .data : AT (__etext)
    {
        __data_start__ = .;
        Image$$RW_IRAM1$$Base = .;
        *(vtable)
        *(.data*)

        . = ALIGN(8);
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

        . = ALIGN(8);
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);

        . = ALIGN(8);
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);

        . = ALIGN(8);
        __data_end__ = .;

    } > RAM

    /* BSS moved to USB_RAM to free main RAM for heap */
    .bss (NOLOAD):
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
        Image$$RW_IRAM1$$ZI$$Limit = . ;
    } > USB_RAM

    /* Heap uses remaining main RAM (after .data) */
    .heap :
    {
        __end__ = .;
        end = __end__;
        *(.heap*)
        . = ORIGIN(RAM) + LENGTH(RAM) - STACK_SIZE;
        __HeapLimit = .;
    } > RAM

    .stack_dummy :
    {
        *(.stack)
    } > RAM

    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    __StackLimit = __StackTop - STACK_SIZE;
    PROVIDE(__stack = __StackTop);

    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

    .AHBSRAM0 (NOLOAD):
    {
        Image$$RW_IRAM2$$Base = . ;
        *(AHBSRAM0)
        Image$$RW_IRAM2$$ZI$$Limit = .;
    } > USB_RAM

    .AHBSRAM1 (NOLOAD):
    {
        Image$$RW_IRAM3$$Base = . ;
        *(AHBSRAM1)
        Image$$RW_IRAM3$$ZI$$Limit = .;
    } > ETH_RAM
}
