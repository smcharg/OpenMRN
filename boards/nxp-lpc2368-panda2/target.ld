OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(entry)

/*INCLUDE "testlibmbed_Debug_lib.ld"*/
GROUP(
 libgcc.a
 libc.a
 libstdc++.a
 libm.a
 /*libcr_newlib_nohost.a*/
 crti.o
 crtn.o
 crtbegin.o
 crtend.o
 )


/* Memory Definitions: */
MEMORY
{
    Flash  (rx)  : ORIGIN = 0x00005000, LENGTH = 460k
    Ram    (rwx) : ORIGIN = 0x40000000, LENGTH = 64k
    UsbRam (rw)  : ORIGIN = 0x7FD00000, LENGTH = 16k
    EthRam (rw)  : ORIGIN = 0x7FE00000, LENGTH = 16k
    CanRam (rw)  : ORIGIN = 0xE0038000, LENGTH = 2k
    BatRam (rw)  : ORIGIN = 0xE0084000, LENGTH = 2k
}

__automata_start = 0x70000;
__automata_end = 0x80000;

/* Stack sizes: */
UND_Stack_Size = 32;
ABT_Stack_Size = 32;
FIQ_Stack_Size = 32;
IRQ_Stack_Size = 0x400;
SVC_Stack_Size = 0x400;
SYS_Stack_Size = 0x800; /*main stack size*/

Stack_Size_Total = UND_Stack_Size + SVC_Stack_Size + ABT_Stack_Size + FIQ_Stack_Size + IRQ_Stack_Size;

/* Stack tops for each mode: */
__stacks_top__ = __ETHRAM_segment_end__;
__und_stack_top__ = __stacks_top__;
__abt_stack_top__ = __und_stack_top__ - UND_Stack_Size ;
__fiq_stack_top__ = __abt_stack_top__ - ABT_Stack_Size ;
__irq_stack_top__ = __fiq_stack_top__ - FIQ_Stack_Size ;
__svc_stack_top__ = __irq_stack_top__ - IRQ_Stack_Size ;
__sys_stack_top__ = __svc_stack_top__ - SVC_Stack_Size ;
__stacks_min__ = __sys_stack_top__ - SYS_Stack_Size;
_sym_use_separate_stack_segment = 1;

/* C-accessible symbols for memory address ranges: */
__FLASH_segment_start__ = ORIGIN( Flash );
__FLASH_segment_end__   = ORIGIN( Flash ) + LENGTH( Flash );
__SRAM_segment_start__  = ORIGIN( Ram );
__SRAM_segment_end__    = ORIGIN( Ram ) + LENGTH( Ram );
__ETHRAM_segment_start__  = ORIGIN( EthRam );
__ETHRAM_segment_end__    = ORIGIN( EthRam ) + LENGTH( EthRam );
__USBRAM_segment_start__  = ORIGIN( UsbRam );
__USBRAM_segment_end__    = ORIGIN( UsbRam ) + LENGTH( UsbRam );



/* This symbol is used by malloc to ensure heap does not run oit of its
allotted space (e.g. by colliding with stacks, or by reaching the end of the
ram segment. */
PROVIDE(__cs3_heap_end = __SRAM_segment_end__);

SECTIONS
{
    /* first section is .text which is used for code */
    __text_start__ = . ;
    .text : {
        __privileged_code_start__ = . ;
        KEEP( *( .vectors ) )
        *( .privileged_code )
    
        __privileged_code_end__ = .;

        . = ALIGN( 4 ) ;
        *( .text .text.* .gnu.linkonce.t.* )
        *( .plt )
        
        . = ALIGN( 4 ) ;
        KEEP( *( .init ) )
        . = ALIGN( 4 ) ;
        KEEP( *( .fini ) )
        . = ALIGN( 4 ) ;
        __preinit_array_start = . ;
        KEEP( *( .preinit_array ) )
        __preinit_array_end = . ;
        . = ALIGN( 4 ) ;
        __init_array_start = . ;
        KEEP( *( SORT( .init_array.* ) ) )
        KEEP( *( .init_array ) )
        __init_array_end = . ;
        
        . = ALIGN( 4 ) ;
        KEEP( *crtbegin.o( .ctors ) )
        KEEP( *( EXCLUDE_FILE( *crtend.o ) .ctors ) )
        KEEP( *( SORT( .ctors.* ) ) )
        KEEP( *crtend.o( .ctors ) )
        
        . = ALIGN( 4 ) ;
        __fini_array_start = . ;
        KEEP( *( .fini_array ) )
        KEEP( *( SORT( .fini_array.* ) ) )
        __fini_array_end = . ;
        
        KEEP( *crtbegin.o( .dtors ) )
        KEEP( *( EXCLUDE_FILE( *crtend.o ) .dtors ) )
        KEEP( *( SORT( .dtors.* ) ) )
        KEEP( *crtend.o( .dtors ) )

        *( .gnu.warning )
        *( .glue_7t ) *( .glue_7 ) *( .vfp11_veneer )

        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)
        
        *( .rodata )
        *(SORT(.rodata.*))
	*(SORT(.gnu.linkonce.r.*))
    } >Flash
    
    __exidx_start = . ;
    .ARM.exidx : {
        *( .ARM.exidx* .gnu.linkonce.armexidx.* )
    } >Flash
    __exidx_end = . ;
    
    .text.align : { . = ALIGN( 8 ) ; } >Flash /* Alignment schenanigans */
    __text_end__ = . ;
    
    /* .bss section -- used for uninitialized data */
    /* Located at the start of RAM */
    .bss (NOLOAD) : {
        __bss_start__ = . ;
        *( .ram_vectors )
        
        __user_bss_start__ = . ;
        *( .user_bss )
        __user_bss_end__ = . ;
        
        *( .shbss )
        *( .bss .bss.* .gnu.linkonce.b.* )
        *( COMMON )
        *( .ram.b )
        . = ALIGN( 8 ) ;
        
        __bss_end__ = . ;
    } >Ram AT>Flash

    /* .data section -- used for initialized data */
    .data : {
	FILL(0xff)
        __data_start__ = . ;
        KEEP( *( .jcr ) )
        *( .got.plt ) *( .got )
        *( .shdata )
        *( .data .data.* .gnu.linkonce.d.* )
	/* this magic is needed for the device tables of openMRN */
        . = ALIGN (8);
        KEEP(*( SORT (.device.table.*))) ;
        . = ALIGN (4);

        *( .ram )
        . = ALIGN( 8 ) ;
        __data_end__ = . ;
    } >Ram AT>Flash

    __data_init_start__ = LOADADDR( .data ) ;

    /* Heap starts here and grows up in memory */
    . = ALIGN( 8 ) ;
    __heap_start__ = . ;
    __cs3_heap_start = .;

	PROVIDE(_pvHeapStart = .);

}
