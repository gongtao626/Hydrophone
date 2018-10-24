# this method initializes debug modules which are not affected by software reset 
# register names should be referenced including the register group name to improve performance 

proc disable_wdt {} {
	# First unlock the watchdog so that we can write to registers */
    # Write 0xC520 to the unlock register WDOG_UNLOCK*/
    reg "Generation 2008 Watchdog Timer (WDOG)/WDOG_UNLOCK" =0xC520
 	
  	# Followed by 0xD928 to complete the unlock */
    reg "Generation 2008 Watchdog Timer (WDOG)/WDOG_UNLOCK" =0xD928
	
	# Clear the WDOGEN bit to disable the watchdog */
    reg "Generation 2008 Watchdog Timer (WDOG)/WDOG_STCTRLH" =0x01D2
}

proc init_debug_modules {} {
    # clear DWT function registers
	reg "Core Debug Registers/DEMCR" = 0x1000001
	reg "Data Watchpoint and Trace Unit Registers/DWT_FUNCTION0" = 0x0
	reg "Data Watchpoint and Trace Unit Registers/DWT_FUNCTION1" = 0x0
	reg "Data Watchpoint and Trace Unit Registers/DWT_FUNCTION2" = 0x0
	reg "Data Watchpoint and Trace Unit Registers/DWT_FUNCTION3" = 0x0
	# clear FPB comparators
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP0" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP1" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP2" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP3" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP4" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP5" = 0x0
}

proc init_trace_modules {} {
    # clear DWT registers
	reg "Data Watchpoint and Trace Unit Registers/DWT_CTRL" =0x40000000
	reg "Data Watchpoint and Trace Unit Registers/DWT_CYCCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_CPICNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_EXCCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_SLEEPCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_LSUCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_FOLDCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_COMP0" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_COMP1" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_COMP2" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_COMP3" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_MASK0" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_MASK1" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_MASK2" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_MASK3" =0x0
	# clear ITM registers
    reg "Instrumentation Trace Macrocell Registers/ITM_LAR" =0xc5acce55
	reg "Instrumentation Trace Macrocell Registers/ITM_TER" =0x0
	reg "Instrumentation Trace Macrocell Registers/ITM_TPR" =0x0
    reg "Instrumentation Trace Macrocell Registers/ITM_TCR" =0x0
    reg "Instrumentation Trace Macrocell Registers/ITM_LAR" =0x1
    # reset Funnel registers
    reg "Embedded Trace Funnel Registers/ETF_FCR" =0x300
    # set SCB_VTOR register for RAM
    reg "System Control Registers/SCB_VTOR" =0x1FFFE000
}

proc envsetup {} {
	# Environment Setup
	radix x 
	config hexprefix 0x
	config MemIdentifier p 
	config MemWidth 32 
	config MemAccess 32 
	config MemSwap off
}

#-------------------------------------------------------------------------------
# Main                                                                          
#-------------------------------------------------------------------------------

  envsetup
  #disable_wdt
  init_debug_modules
  init_trace_modules
  