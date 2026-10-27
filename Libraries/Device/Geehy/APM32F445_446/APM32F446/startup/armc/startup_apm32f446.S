;
; modified by ARM
;

;/*!
; * @file        startup_apm32f446.S
; *
; * @brief       ARM Compiler Collection Startup File
; *
; * @version     V1.0.0
; *
; * @date        2026-01-31
; *
; * @attention
; *
; *  Copyright (C) 2026 Geehy Semiconductor
; *
; *  You may not use this file except in compliance with the
; *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
; *
; *  The program is only for reference, which is distributed in the hope
; *  that it will be useful and instructional for customers to develop
; *  their software. Unless required by applicable law or agreed to in
; *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
; *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
; *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
; *  and limitations under the License.
; */

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    |.text|, CODE, READONLY
                EXPORT  __Vectors

                EXPORT __ROM_VECTOR_START
                EXPORT __ROM_VECTOR_END
                EXPORT __VECTOR_ROM
                EXPORT __RAM_DATA_START
                EXPORT __RAM_DATA_END
                EXPORT __VECTOR_RAM
                EXPORT __RAM_VECTOR_TABLE_SIZE

                EXPORT __BSS_START
                EXPORT __BSS_END

                EXPORT __RAM_CODE
                EXPORT __ROM_CODE
                EXPORT __CODE_END

                EXPORT __DATA_RAM
                EXPORT __DATA_ROM
                EXPORT __DATA_END

                EXPORT __RAM_START
                EXPORT __RAM_END
                EXPORT __RAM_CODE_START
                EXPORT __RAM_CODE_END
                EXPORT __USER_SECTION_START
                EXPORT __USER_SECTION_END

                IMPORT |Image$$ARM_LIB_STACK$$ZI$$Limit|
                IMPORT |Image$$ARM_LIB_STACK$$Limit|
                IMPORT |Image$$RW_DATA_2$$ZI$$Base|
                IMPORT |Image$$RW_DATA_2$$ZI$$Limit|

                IMPORT |Image$$TEXT_CONST$$Base|
                IMPORT |Image$$TEXT_CONST$$Limit|

                IMPORT |Image$$RW_DATA$$Base|
                IMPORT |Image$$RW_DATA$$Length|
                IMPORT |Image$$RW_DATA$$Limit|

                IMPORT |Image$$RW_CODE$$Base|
                IMPORT |Image$$RW_CODE$$Length|
                IMPORT |Image$$RW_CODE$$Limit|

                IMPORT |Image$$TEXT$$Limit|

                IMPORT |Image$$VECTOR_RAM$$Base|
                IMPORT |Image$$VECTOR_RAM$$Length|

                IMPORT |Image$$VECTOR_ROM$$Base|
                IMPORT |Image$$VECTOR_ROM$$Length|
                IMPORT |Image$$VECTOR_ROM$$Limit|

                IMPORT |Image$$RW_USER_SECTION$$Base|
                IMPORT |Image$$RW_USER_SECTION$$Limit|

; Interrupt vector table start and end address in RAM/ROM
__ROM_VECTOR_END DCD |Image$$VECTOR_ROM$$Limit|
__ROM_VECTOR_START DCD |Image$$VECTOR_ROM$$Base|
__VECTOR_ROM DCD |Image$$VECTOR_ROM$$Base|
__VECTOR_RAM DCD |Image$$VECTOR_RAM$$Base|
__RAM_VECTOR_TABLE_SIZE DCD |Image$$VECTOR_ROM$$Length|

; .text section start and end address in RAM/ROM
__RAM_CODE DCD |Image$$RW_CODE$$Base|
__RAM_CODE_END DCD |Image$$RW_CODE$$Limit|
__RAM_CODE_START DCD |Image$$RW_CODE$$Base|
__ROM_CODE DCD  |Image$$RW_DATA$$Length|+ |Image$$TEXT$$Limit|
__CODE_END DCD  |Image$$RW_DATA$$Length|+ |Image$$TEXT$$Limit| + |Image$$RW_CODE$$Length|

; .bss section start and end address in RAM
__BSS_END DCD  |Image$$RW_DATA_2$$ZI$$Limit|
__BSS_START DCD  |Image$$RW_DATA_2$$ZI$$Base|

; .data section start and end address in RAM/ROM
__DATA_RAM DCD  |Image$$RW_DATA$$Base|
__RAM_DATA_END DCD  |Image$$RW_DATA$$Limit|
__RAM_DATA_START DCD  |Image$$RW_DATA$$Base|
__DATA_END DCD  |Image$$RW_DATA$$Length|+ |Image$$TEXT$$Limit|
__DATA_ROM DCD  |Image$$TEXT$$Limit|

; user define section start and end address
__USER_SECTION_END DCD |Image$$RW_USER_SECTION$$Limit|
__USER_SECTION_START DCD |Image$$RW_USER_SECTION$$Base|

#ifndef __flash_vector_table__
__RAM_START DCD |Image$$VECTOR_RAM$$Base|
#else
__RAM_START DCD |Image$$RW_DATA$$Base|
#endif
__RAM_END   DCD |Image$$ARM_LIB_STACK$$Limit|

        AREA    RESET, DATA, READONLY
__Vectors       DCD     |Image$$ARM_LIB_STACK$$ZI$$Limit|   ; Top of Stack
                DCD     Reset_Handler                       ; Reset Handler
                DCD     NMI_Handler                         ; NMI Handler
                DCD     HardFault_Handler                   ; Hard Fault Handler
                DCD     MemManage_Handler                   ; MPU Fault Handler
                DCD     BusFault_Handler                    ; Bus Fault Handler
                DCD     UsageFault_Handler                  ; Usage Fault Handler
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     SVC_Handler                         ; SVCall Handler
                DCD     DebugMon_Handler                    ; Debug Monitor Handler
                DCD     0                                   ; Reserved
                DCD     PendSV_Handler                      ; PendSV Handler
                DCD     SysTick_Handler                     ; SysTick Handler

                ; External Interrupts
                DCD   DMA0_IRQHandler                       ; DMA channel 0 transfer complete
                DCD   DMA1_IRQHandler                       ; DMA channel 1 transfer complete
                DCD   DMA2_IRQHandler                       ; DMA channel 2 transfer complete
                DCD   DMA3_IRQHandler                       ; DMA channel 3 transfer complete
                DCD   DMA4_IRQHandler                       ; DMA channel 4 transfer complete
                DCD   DMA5_IRQHandler                       ; DMA channel 5 transfer complete
                DCD   DMA6_IRQHandler                       ; DMA channel 6 transfer complete
                DCD   DMA7_IRQHandler                       ; DMA channel 7 transfer complete
                DCD   DMA8_IRQHandler                       ; DMA channel 8 transfer complete
                DCD   DMA9_IRQHandler                       ; DMA channel 9 transfer complete
                DCD   DMA10_IRQHandler                      ; DMA channel 10 transfer complete
                DCD   DMA11_IRQHandler                      ; DMA channel 11 transfer complete
                DCD   DMA12_IRQHandler                      ; DMA channel 12 transfer complete
                DCD   DMA13_IRQHandler                      ; DMA channel 13 transfer complete
                DCD   DMA14_IRQHandler                      ; DMA channel 14 transfer complete
                DCD   DMA15_IRQHandler                      ; DMA channel 15 transfer complete
                DCD   DMA_Error_IRQHandler                  ; DMA error interrupt channels 0-15
                DCD   MC_IRQHandler                         ; FPU sources
                DCD   FLASH_IRQHandler                      ; FLASH Command complete
                DCD   Read_Collision_IRQHandler             ; FLASH Read collision
                DCD   LVD_LVW_IRQHandler                    ; PMU Low voltage detect interrupt
                DCD   FLASH_Fault_IRQHandler                ; FLASH Double bit fault detect
                DCD   WDT_EWDT_IRQHandler                   ; Single interrupt vector for WDT and EWDT
                DCD   RMU_IRQHandler                        ; RMU Asynchronous Interrupt
                DCD   LPI2C_Master_IRQHandler               ; LPI2C Master Interrupt
                DCD   LPI2C_Slave_IRQHandler                ; LPI2C Slave Interrupt
                DCD   LPSPI0_IRQHandler                     ; LPSPI0 Interrupt
                DCD   LPSPI1_IRQHandler                     ; LPSPI1 Interrupt
                DCD   LPSPI2_IRQHandler                     ; LPSPI2 Interrupt
                DCD   Reserved45_IRQHandler                 ; Reserved Interrupt 45
                DCD   Reserved46_IRQHandler                 ; Reserved Interrupt 46
                DCD   LPUART0_RxTx_IRQHandler               ; LPUART0 Transmit / Receive Interrupt
                DCD   Reserved48_IRQHandler                 ; Reserved Interrupt 48
                DCD   LPUART1_RxTx_IRQHandler               ; LPUART1 Transmit / Receive  Interrupt
                DCD   Reserved50_IRQHandler                 ; Reserved Interrupt 50
                DCD   LPUART2_RxTx_IRQHandler               ; LPUART2 Transmit / Receive  Interrupt
                DCD   Reserved52_IRQHandler                 ; Reserved Interrupt 52
                DCD   Reserved53_IRQHandler                 ; Reserved Interrupt 53
                DCD   Reserved54_IRQHandler                 ; Reserved Interrupt 54
                DCD   ADC0_IRQHandler                       ; ADC0 interrupt request.
                DCD   ADC1_IRQHandler                       ; ADC1 interrupt request.
                DCD   COMP_IRQHandler                       ; COMP interrupt request
                DCD   Reserved58_IRQHandler                 ; Reserved Interrupt 58
                DCD   Reserved59_IRQHandler                 ; Reserved Interrupt 59
                DCD   EREP_single_fault_IRQHandler          ; EREP single bit error correction
                DCD   EREP_double_fault_IRQHandler          ; EREP double bit error non-correctable
                DCD   RTC_IRQHandler                        ; RTC alarm interrupt
                DCD   RTC_Seconds_IRQHandler                ; RTC seconds interrupt
                DCD   LPITMR_Ch0_IRQHandler                 ; LPITMR channel 0 overflow interrupt
                DCD   LPITMR_Ch1_IRQHandler                 ; LPITMR channel 1 overflow interrupt
                DCD   LPITMR_Ch2_IRQHandler                 ; LPITMR channel 2 overflow interrupt
                DCD   LPITMR_Ch3_IRQHandler                 ; LPITMR channel 3 overflow interrupt
                DCD   PDU0_IRQHandler                       ; PDU0 interrupt
                DCD   Reserved69_IRQHandler                 ; Reserved Interrupt 69
                DCD   Reserved70_IRQHandler                 ; Reserved Interrupt 70
                DCD   Reserved71_IRQHandler                 ; Reserved Interrupt 71
                DCD   Reserved72_IRQHandler                 ; Reserved Interrupt 72
                DCD   SCG_IRQHandler                        ; SCG bus interrupt request
                DCD   LPTMR_IRQHandler                      ; LPTMR interrupt request
                DCD   PMA_IRQHandler                        ; PM A pin detect interrupt
                DCD   PMB_IRQHandler                        ; PM B pin detect interrupt
                DCD   PMC_IRQHandler                        ; PM C pin detect interrupt
                DCD   PMD_IRQHandler                        ; PM D pin detect interrupt
                DCD   PME_IRQHandler                        ; PM E pin detect interrupt
                DCD   SWI_IRQHandler                        ; Software interrupt
                DCD   Reserved81_IRQHandler                 ; Reserved Interrupt 81
                DCD   Reserved82_IRQHandler                 ; Reserved Interrupt 82
                DCD   Reserved83_IRQHandler                 ; Reserved Interrupt 83
                DCD   PDU1_IRQHandler                       ; PDU1 interrupt
                DCD   CFGIO_IRQHandler                    ; CFGIO Interrupt
                DCD   Reserved86_IRQHandler                 ; Reserved Interrupt 86
                DCD   Reserved87_IRQHandler                 ; Reserved Interrupt 87
                DCD   Reserved88_IRQHandler                 ; Reserved Interrupt 88
                DCD   Reserved89_IRQHandler                 ; Reserved Interrupt 89
                DCD   Reserved90_IRQHandler                 ; Reserved Interrupt 90
                DCD   Reserved91_IRQHandler                 ; Reserved Interrupt 91
                DCD   Reserved92_IRQHandler                 ; Reserved Interrupt 92
                DCD   Reserved93_IRQHandler                 ; Reserved Interrupt 93
                DCD   CAN0_ORed_IRQHandler              ; CAN0 ORed
                DCD   CAN0_Error_IRQHandler             ; CAN0 Interrupt indicating that errors were detected on the CAN bus
                DCD   CAN0_Wake_Up_IRQHandler           ; CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode
                DCD   CAN0_ORed_0_15_MB_IRQHandler      ; CAN0 ORed Message buffer (0-15)
                DCD   CAN0_ORed_16_31_MB_IRQHandler     ; CAN0 ORed Message buffer (16-31)
                DCD   Reserved99_IRQHandler                 ; Reserved Interrupt 99
                DCD   Reserved100_IRQHandler                ; Reserved Interrupt 100
                DCD   CAN1_ORed_IRQHandler              ; CAN1 ORed
                DCD   CAN1_Error_IRQHandler             ; CAN1 Interrupt indicating that errors were detected on the CAN bus
                DCD   Reserved103_IRQHandler                ; Reserved Interrupt 103
                DCD   CAN1_ORed_0_15_MB_IRQHandler      ; CAN1 ORed Interrupt for Message buffer (0-15)
                DCD   CAN1_ORed_16_31_MB_IRQHandler     ; CAN1 ORed Interrupt for Message buffer (16-31)
                DCD   Reserved106_IRQHandler                ; Reserved Interrupt 106
                DCD   Reserved107_IRQHandler                ; Reserved Interrupt 107
                DCD   CAN2_ORed_IRQHandler              ; CAN2 ORed
                DCD   CAN2_Error_IRQHandler             ; CAN2 Interrupt indicating that errors were detected on the CAN bus
                DCD   Reserved110_IRQHandler                ; Reserved Interrupt 110
                DCD   CAN2_ORed_0_15_MB_IRQHandler      ; CAN2 ORed Message buffer (0-15)
                DCD   Reserved112_IRQHandler                ; Reserved Interrupt 112
                DCD   Reserved113_IRQHandler                ; Reserved Interrupt 113
                DCD   Reserved114_IRQHandler                ; Reserved Interrupt 114
                DCD   CFGTMR0_Ch0_Ch1_IRQHandler              ; CFGTMR0 Channel 0 and 1 interrupt
                DCD   CFGTMR0_Ch2_Ch3_IRQHandler              ; CFGTMR0 Channel 2 and 3 interrupt
                DCD   CFGTMR0_Ch4_Ch5_IRQHandler              ; CFGTMR0 Channel 4 and 5 interrupt
                DCD   CFGTMR0_Ch6_Ch7_IRQHandler              ; CFGTMR0 Channel 6 and 7 interrupt
                DCD   CFGTMR0_Fault_IRQHandler                ; CFGTMR0 Fault interrupt
                DCD   CFGTMR0_Ovf_Reload_IRQHandler           ; CFGTMR0 Counter overflow and Reload interrupt
                DCD   CFGTMR1_Ch0_Ch1_IRQHandler              ; CFGTMR1 Channel 0 and 1 interrupt
                DCD   CFGTMR1_Ch2_Ch3_IRQHandler              ; CFGTMR1 Channel 2 and 3 interrupt
                DCD   CFGTMR1_Ch4_Ch5_IRQHandler              ; CFGTMR1 Channel 4 and 5 interrupt
                DCD   CFGTMR1_Ch6_Ch7_IRQHandler              ; CFGTMR1 Channel 6 and 7 interrupt
                DCD   CFGTMR1_Fault_IRQHandler                ; CFGTMR1 Fault interrupt
                DCD   CFGTMR1_Ovf_Reload_IRQHandler           ; CFGTMR1 Counter overflow and Reload interrupt
                DCD   CFGTMR2_Ch0_Ch1_IRQHandler              ; CFGTMR2 Channel 0 and 1 interrupt
                DCD   CFGTMR2_Ch2_Ch3_IRQHandler              ; CFGTMR2 Channel 2 and 3 interrupt
                DCD   CFGTMR2_Ch4_Ch5_IRQHandler              ; CFGTMR2 Channel 4 and 5 interrupt
                DCD   CFGTMR2_Ch6_Ch7_IRQHandler              ; CFGTMR2 Channel 6 and 7 interrupt
                DCD   CFGTMR2_Fault_IRQHandler                ; CFGTMR2 Fault interrupt
                DCD   CFGTMR2_Ovf_Reload_IRQHandler           ; CFGTMR2 Counter overflow and Reload interrupt
                DCD   CFGTMR3_Ch0_Ch1_IRQHandler              ; CFGTMR3 Channel 0 and 1 interrupt
                DCD   CFGTMR3_Ch2_Ch3_IRQHandler              ; CFGTMR3 Channel 2 and 3 interrupt
                DCD   CFGTMR3_Ch4_Ch5_IRQHandler              ; CFGTMR3 Channel 4 and 5 interrupt
                DCD   CFGTMR3_Ch6_Ch7_IRQHandler              ; CFGTMR3 Channel 6 and 7 interrupt
                DCD   CFGTMR3_Fault_IRQHandler                ; CFGTMR3 Fault interrupt
                DCD   CFGTMR3_Ovf_Reload_IRQHandler           ; CFGTMR3 Counter overflow and Reload interrupt
                DCD   CFGTMR4_Ch0_Ch1_IRQHandler              ; CFGTMR4 Channel 0 and 1 interrupt
                DCD   CFGTMR4_Ch2_Ch3_IRQHandler              ; CFGTMR4 Channel 2 and 3 interrupt
                DCD   CFGTMR4_Ch4_Ch5_IRQHandler              ; CFGTMR4 Channel 4 and 5 interrupt
                DCD   CFGTMR4_Ch6_Ch7_IRQHandler              ; CFGTMR4 Channel 6 and 7 interrupt
                DCD   CFGTMR4_Fault_IRQHandler                ; CFGTMR4 Fault interrupt
                DCD   CFGTMR4_Ovf_Reload_IRQHandler           ; CFGTMR4 Counter overflow and Reload interrupt
                DCD   CFGTMR5_Ch0_Ch1_IRQHandler              ; CFGTMR5 Channel 0 and 1 interrupt
                DCD   CFGTMR5_Ch2_Ch3_IRQHandler              ; CFGTMR5 Channel 2 and 3 interrupt
                DCD   CFGTMR5_Ch4_Ch5_IRQHandler              ; CFGTMR5 Channel 4 and 5 interrupt
                DCD   CFGTMR5_Ch6_Ch7_IRQHandler              ; CFGTMR5 Channel 6 and 7 interrupt
                DCD   CFGTMR5_Fault_IRQHandler                ; CFGTMR5 Fault interrupt
                DCD   CFGTMR5_Ovf_Reload_IRQHandler           ; CFGTMR5 Counter overflow and Reload interrupt
                DCD   DefaultISR                            ; Default ISR
__Vectors_End
__Vectors_Size  EQU  __Vectors_End - __Vectors

; Flash Configuration
; Backdoor Comparison KEY Configuration
BackDoorKey0    EQU     0xFF
BackDoorKey1    EQU     0xFF
BackDoorKey2    EQU     0xFF
BackDoorKey3    EQU     0xFF
BackDoorKey4    EQU     0xFF
BackDoorKey5    EQU     0xFF
BackDoorKey6    EQU     0xFF
BackDoorKey7    EQU     0xFF

; Program flash protection bytes (PFPROx)
; PFPRO0
nPFPRO0         EQU     0x00
PFPRO0          EQU     nPFPRO0:EOR:0xFF

; PFPRO1
nPFPRO1         EQU     0x00
PFPRO1          EQU     nPFPRO1:EOR:0xFF

; PFPRO2
nPFPRO2         EQU     0x00
PFPRO2          EQU     nPFPRO2:EOR:0xFF

; PFPRO3
nPFPRO3         EQU     0x00
PFPRO3          EQU     nPFPRO3:EOR:0xFF

; Data flash protection byte (DFPRO)
nDFPRO          EQU     0x00
DFPRO           EQU     nDFPRO:EOR:0xFF

; EEPROM protection byte (EEPRO)
nEEPRO          EQU     0x00
EEPRO           EQU     nEEPRO:EOR:0xFF

; Flash nonvolatile option byte (NOPT)
NOPT            EQU     0xFF

; Flash security byte (SEC)
SEC             EQU     0xFE


#ifndef RAM_TARGET
                AREA    FlashConfig, DATA, READONLY
__FlashConfig
                DCB     BackDoorKey0, BackDoorKey1, BackDoorKey2, BackDoorKey3
                DCB     BackDoorKey4, BackDoorKey5, BackDoorKey6, BackDoorKey7
                DCB     PFPRO0      , PFPRO1      , PFPRO2      , PFPRO3
                DCB     SEC         , NOPT        , EEPRO       , DFPRO
#endif


                AREA    |.text|, CODE, READONLY


Reset_Handler   PROC
                ALIGN   4
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  bss_data_init
                IMPORT  main
                IMPORT  __main

#ifndef RAM_TARGET
                REQUIRE FlashConfig
#endif

                CPSID   I

#ifdef START_FROM_FLASH
                LDR     R0, =__RAM_START
                LDR     R0, [R0]
                LDR     R1, =__RAM_END
                LDR     R1, [R1]
                SUB     R1, R1, R0
                SUB     R1, R1, #0x1
                CMP     R1, #0x1
                BLE     Loop_0

                MOV     R2, #0x0
Loop_1
                STR     R2, [R0], #0x4
                SUB     R1, R1, #0x4
                CMP     R1, #0x4
                BGE     Loop_1

Loop_0
                NOP
#endif
                ; Set SCB VTOR Register
                LDR     R1, =0xE000ED08
                LDR     R2, =__Vectors
                STR     R2, [R1]

                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =bss_data_init
                BLX     R0

                CPSIE   I
                LDR     R0, =__main
                BX      R0
                ENDP

NMI_Handler\
                PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler\
                PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP
DefaultISR\
                PROC
                EXPORT  DefaultISR                [WEAK]
                B       .
                ENDP
Default_Handler PROC
                EXPORT  DMA0_IRQHandler                      [WEAK]
                EXPORT  DMA1_IRQHandler                      [WEAK]
                EXPORT  DMA2_IRQHandler                      [WEAK]
                EXPORT  DMA3_IRQHandler                      [WEAK]
                EXPORT  DMA4_IRQHandler                      [WEAK]
                EXPORT  DMA5_IRQHandler                      [WEAK]
                EXPORT  DMA6_IRQHandler                      [WEAK]
                EXPORT  DMA7_IRQHandler                      [WEAK]
                EXPORT  DMA8_IRQHandler                      [WEAK]
                EXPORT  DMA9_IRQHandler                      [WEAK]
                EXPORT  DMA10_IRQHandler                     [WEAK]
                EXPORT  DMA11_IRQHandler                     [WEAK]
                EXPORT  DMA12_IRQHandler                     [WEAK]
                EXPORT  DMA13_IRQHandler                     [WEAK]
                EXPORT  DMA14_IRQHandler                     [WEAK]
                EXPORT  DMA15_IRQHandler                     [WEAK]
                EXPORT  DMA_Error_IRQHandler                 [WEAK]
                EXPORT  MC_IRQHandler                        [WEAK]
                EXPORT  FLASH_IRQHandler                     [WEAK]
                EXPORT  Read_Collision_IRQHandler            [WEAK]
                EXPORT  LVD_LVW_IRQHandler                   [WEAK]
                EXPORT  FLASH_Fault_IRQHandler               [WEAK]
                EXPORT  WDT_EWDT_IRQHandler                  [WEAK]
                EXPORT  RMU_IRQHandler                       [WEAK]
                EXPORT  LPI2C_Master_IRQHandler              [WEAK]
                EXPORT  LPI2C_Slave_IRQHandler               [WEAK]
                EXPORT  LPSPI0_IRQHandler                    [WEAK]
                EXPORT  LPSPI1_IRQHandler                    [WEAK]
                EXPORT  LPSPI2_IRQHandler                    [WEAK]
                EXPORT  Reserved45_IRQHandler                [WEAK]
                EXPORT  Reserved46_IRQHandler                [WEAK]
                EXPORT  LPUART0_RxTx_IRQHandler              [WEAK]
                EXPORT  Reserved48_IRQHandler                [WEAK]
                EXPORT  LPUART1_RxTx_IRQHandler              [WEAK]
                EXPORT  Reserved50_IRQHandler                [WEAK]
                EXPORT  LPUART2_RxTx_IRQHandler              [WEAK]
                EXPORT  Reserved52_IRQHandler                [WEAK]
                EXPORT  Reserved53_IRQHandler                [WEAK]
                EXPORT  Reserved54_IRQHandler                [WEAK]
                EXPORT  ADC0_IRQHandler                      [WEAK]
                EXPORT  ADC1_IRQHandler                      [WEAK]
                EXPORT  COMP_IRQHandler                      [WEAK]
                EXPORT  Reserved58_IRQHandler                [WEAK]
                EXPORT  Reserved59_IRQHandler                [WEAK]
                EXPORT  EREP_single_fault_IRQHandler         [WEAK]
                EXPORT  EREP_double_fault_IRQHandler         [WEAK]
                EXPORT  RTC_IRQHandler                       [WEAK]
                EXPORT  RTC_Seconds_IRQHandler               [WEAK]
                EXPORT  LPITMR_Ch0_IRQHandler                [WEAK]
                EXPORT  LPITMR_Ch1_IRQHandler                [WEAK]
                EXPORT  LPITMR_Ch2_IRQHandler                [WEAK]
                EXPORT  LPITMR_Ch3_IRQHandler                [WEAK]
                EXPORT  PDU0_IRQHandler                      [WEAK]
                EXPORT  Reserved69_IRQHandler                [WEAK]
                EXPORT  Reserved70_IRQHandler                [WEAK]
                EXPORT  Reserved71_IRQHandler                [WEAK]
                EXPORT  Reserved72_IRQHandler                [WEAK]
                EXPORT  SCG_IRQHandler                       [WEAK]
                EXPORT  LPTMR_IRQHandler                     [WEAK]
                EXPORT  PMA_IRQHandler                       [WEAK]
                EXPORT  PMB_IRQHandler                       [WEAK]
                EXPORT  PMC_IRQHandler                       [WEAK]
                EXPORT  PMD_IRQHandler                       [WEAK]
                EXPORT  PME_IRQHandler                       [WEAK]
                EXPORT  SWI_IRQHandler                       [WEAK]
                EXPORT  Reserved81_IRQHandler                [WEAK]
                EXPORT  Reserved82_IRQHandler                [WEAK]
                EXPORT  Reserved83_IRQHandler                [WEAK]
                EXPORT  PDU1_IRQHandler                      [WEAK]
                EXPORT  CFGIO_IRQHandler                   [WEAK]
                EXPORT  Reserved86_IRQHandler                [WEAK]
                EXPORT  Reserved87_IRQHandler                [WEAK]
                EXPORT  Reserved88_IRQHandler                [WEAK]
                EXPORT  Reserved89_IRQHandler                [WEAK]
                EXPORT  Reserved90_IRQHandler                [WEAK]
                EXPORT  Reserved91_IRQHandler                [WEAK]
                EXPORT  Reserved92_IRQHandler                [WEAK]
                EXPORT  Reserved93_IRQHandler                [WEAK]
                EXPORT  CAN0_ORed_IRQHandler             [WEAK]
                EXPORT  CAN0_Error_IRQHandler            [WEAK]
                EXPORT  CAN0_Wake_Up_IRQHandler          [WEAK]
                EXPORT  CAN0_ORed_0_15_MB_IRQHandler     [WEAK]
                EXPORT  CAN0_ORed_16_31_MB_IRQHandler    [WEAK]
                EXPORT  Reserved99_IRQHandler                [WEAK]
                EXPORT  Reserved100_IRQHandler               [WEAK]
                EXPORT  CAN1_ORed_IRQHandler             [WEAK]
                EXPORT  CAN1_Error_IRQHandler            [WEAK]
                EXPORT  Reserved103_IRQHandler               [WEAK]
                EXPORT  CAN1_ORed_0_15_MB_IRQHandler     [WEAK]
                EXPORT  CAN1_ORed_16_31_MB_IRQHandler    [WEAK]
                EXPORT  Reserved106_IRQHandler               [WEAK]
                EXPORT  Reserved107_IRQHandler               [WEAK]
                EXPORT  CAN2_ORed_IRQHandler             [WEAK]
                EXPORT  CAN2_Error_IRQHandler            [WEAK]
                EXPORT  Reserved110_IRQHandler               [WEAK]
                EXPORT  CAN2_ORed_0_15_MB_IRQHandler     [WEAK]
                EXPORT  Reserved112_IRQHandler               [WEAK]
                EXPORT  Reserved113_IRQHandler               [WEAK]
                EXPORT  Reserved114_IRQHandler               [WEAK]
                EXPORT  CFGTMR0_Ch0_Ch1_IRQHandler             [WEAK]
                EXPORT  CFGTMR0_Ch2_Ch3_IRQHandler             [WEAK]
                EXPORT  CFGTMR0_Ch4_Ch5_IRQHandler             [WEAK]
                EXPORT  CFGTMR0_Ch6_Ch7_IRQHandler             [WEAK]
                EXPORT  CFGTMR0_Fault_IRQHandler               [WEAK]
                EXPORT  CFGTMR0_Ovf_Reload_IRQHandler          [WEAK]
                EXPORT  CFGTMR1_Ch0_Ch1_IRQHandler             [WEAK]
                EXPORT  CFGTMR1_Ch2_Ch3_IRQHandler             [WEAK]
                EXPORT  CFGTMR1_Ch4_Ch5_IRQHandler             [WEAK]
                EXPORT  CFGTMR1_Ch6_Ch7_IRQHandler             [WEAK]
                EXPORT  CFGTMR1_Fault_IRQHandler               [WEAK]
                EXPORT  CFGTMR1_Ovf_Reload_IRQHandler          [WEAK]
                EXPORT  CFGTMR2_Ch0_Ch1_IRQHandler             [WEAK]
                EXPORT  CFGTMR2_Ch2_Ch3_IRQHandler             [WEAK]
                EXPORT  CFGTMR2_Ch4_Ch5_IRQHandler             [WEAK]
                EXPORT  CFGTMR2_Ch6_Ch7_IRQHandler             [WEAK]
                EXPORT  CFGTMR2_Fault_IRQHandler               [WEAK]
                EXPORT  CFGTMR2_Ovf_Reload_IRQHandler          [WEAK]
                EXPORT  CFGTMR3_Ch0_Ch1_IRQHandler             [WEAK]
                EXPORT  CFGTMR3_Ch2_Ch3_IRQHandler             [WEAK]
                EXPORT  CFGTMR3_Ch4_Ch5_IRQHandler             [WEAK]
                EXPORT  CFGTMR3_Ch6_Ch7_IRQHandler             [WEAK]
                EXPORT  CFGTMR3_Fault_IRQHandler               [WEAK]
                EXPORT  CFGTMR3_Ovf_Reload_IRQHandler          [WEAK]
                EXPORT  CFGTMR4_Ch0_Ch1_IRQHandler             [WEAK]
                EXPORT  CFGTMR4_Ch2_Ch3_IRQHandler             [WEAK]
                EXPORT  CFGTMR4_Ch4_Ch5_IRQHandler             [WEAK]
                EXPORT  CFGTMR4_Ch6_Ch7_IRQHandler             [WEAK]
                EXPORT  CFGTMR4_Fault_IRQHandler               [WEAK]
                EXPORT  CFGTMR4_Ovf_Reload_IRQHandler          [WEAK]
                EXPORT  CFGTMR5_Ch0_Ch1_IRQHandler             [WEAK]
                EXPORT  CFGTMR5_Ch2_Ch3_IRQHandler             [WEAK]
                EXPORT  CFGTMR5_Ch4_Ch5_IRQHandler             [WEAK]
                EXPORT  CFGTMR5_Ch6_Ch7_IRQHandler             [WEAK]
                EXPORT  CFGTMR5_Fault_IRQHandler               [WEAK]
                EXPORT  CFGTMR5_Ovf_Reload_IRQHandler          [WEAK]
DMA0_IRQHandler                                  ; DMA channel 0 transfer complete
DMA1_IRQHandler                                  ; DMA channel 1 transfer complete
DMA2_IRQHandler                                  ; DMA channel 2 transfer complete
DMA3_IRQHandler                                  ; DMA channel 3 transfer complete
DMA4_IRQHandler                                  ; DMA channel 4 transfer complete
DMA5_IRQHandler                                  ; DMA channel 5 transfer complete
DMA6_IRQHandler                                  ; DMA channel 6 transfer complete
DMA7_IRQHandler                                  ; DMA channel 7 transfer complete
DMA8_IRQHandler                                  ; DMA channel 8 transfer complete
DMA9_IRQHandler                                  ; DMA channel 9 transfer complete
DMA10_IRQHandler                                 ; DMA channel 10 transfer complete
DMA11_IRQHandler                                 ; DMA channel 11 transfer complete
DMA12_IRQHandler                                 ; DMA channel 12 transfer complete
DMA13_IRQHandler                                 ; DMA channel 13 transfer complete
DMA14_IRQHandler                                 ; DMA channel 14 transfer complete
DMA15_IRQHandler                                 ; DMA channel 15 transfer complete
DMA_Error_IRQHandler                             ; DMA error interrupt channels 0-15
MC_IRQHandler                                    ; FPU sources
FLASH_IRQHandler                                 ; FLASH Command complete
Read_Collision_IRQHandler                        ; FLASH Read collision
LVD_LVW_IRQHandler                               ; PMC Low voltage detect interrupt
FLASH_Fault_IRQHandler                           ; FLASH Double bit fault detect
WDT_EWDT_IRQHandler                              ; Single interrupt vector for WDT and EWDT
RMU_IRQHandler                                   ; RMU Asynchronous Interrupt
LPI2C_Master_IRQHandler                          ; LPI2C Master Interrupt
LPI2C_Slave_IRQHandler                           ; LPI2C Slave Interrupt
LPSPI0_IRQHandler                                ; LPSPI0 Interrupt
LPSPI1_IRQHandler                                ; LPSPI1 Interrupt
LPSPI2_IRQHandler                                ; LPSPI2 Interrupt
Reserved45_IRQHandler                            ; Reserved Interrupt 45
Reserved46_IRQHandler                            ; Reserved Interrupt 46
LPUART0_RxTx_IRQHandler                          ; LPUART0 Transmit / Receive Interrupt
Reserved48_IRQHandler                            ; Reserved Interrupt 48
LPUART1_RxTx_IRQHandler                          ; LPUART1 Transmit / Receive  Interrupt
Reserved50_IRQHandler                            ; Reserved Interrupt 50
LPUART2_RxTx_IRQHandler                          ; LPUART2 Transmit / Receive  Interrupt
Reserved52_IRQHandler                            ; Reserved Interrupt 52
Reserved53_IRQHandler                            ; Reserved Interrupt 53
Reserved54_IRQHandler                            ; Reserved Interrupt 54
ADC0_IRQHandler                                  ; ADC0 interrupt request.
ADC1_IRQHandler                                  ; ADC1 interrupt request.
COMP_IRQHandler                                  ; COMP interrupt request
Reserved58_IRQHandler                            ; Reserved Interrupt 58
Reserved59_IRQHandler                            ; Reserved Interrupt 59
EREP_single_fault_IRQHandler                     ; EREP single bit error correction
EREP_double_fault_IRQHandler                     ; EREP double bit error non-correctable
RTC_IRQHandler                                   ; RTC alarm interrupt
RTC_Seconds_IRQHandler                           ; RTC seconds interrupt
LPITMR_Ch0_IRQHandler                            ; LPITMR channel 0 overflow interrupt
LPITMR_Ch1_IRQHandler                            ; LPITMR channel 1 overflow interrupt
LPITMR_Ch2_IRQHandler                            ; LPITMR channel 2 overflow interrupt
LPITMR_Ch3_IRQHandler                            ; LPITMR channel 3 overflow interrupt
PDU0_IRQHandler                                  ; PDU0 interrupt
Reserved69_IRQHandler                            ; Reserved Interrupt 69
Reserved70_IRQHandler                            ; Reserved Interrupt 70
Reserved71_IRQHandler                            ; Reserved Interrupt 71
Reserved72_IRQHandler                            ; Reserved Interrupt 72
SCG_IRQHandler                                   ; SCG bus interrupt request
LPTMR_IRQHandler                                 ; LPTMR interrupt request
PMA_IRQHandler                                   ; PM A pin detect interrupt
PMB_IRQHandler                                   ; PM B pin detect interrupt
PMC_IRQHandler                                   ; PM C pin detect interrupt
PMD_IRQHandler                                   ; PM D pin detect interrupt
PME_IRQHandler                                   ; PM E pin detect interrupt
SWI_IRQHandler                                   ; Software interrupt
Reserved81_IRQHandler                            ; Reserved Interrupt 81
Reserved82_IRQHandler                            ; Reserved Interrupt 82
Reserved83_IRQHandler                            ; Reserved Interrupt 83
PDU1_IRQHandler                                  ; PDU1 interrupt
CFGIO_IRQHandler                               ; CFGIO Interrupt
Reserved86_IRQHandler                            ; Reserved Interrupt 86
Reserved87_IRQHandler                            ; Reserved Interrupt 87
Reserved88_IRQHandler                            ; Reserved Interrupt 88
Reserved89_IRQHandler                            ; Reserved Interrupt 89
Reserved90_IRQHandler                            ; Reserved Interrupt 90
Reserved91_IRQHandler                            ; Reserved Interrupt 91
Reserved92_IRQHandler                            ; Reserved Interrupt 92
Reserved93_IRQHandler                            ; Reserved Interrupt 93
CAN0_ORed_IRQHandler                         ; CAN0 ORed
CAN0_Error_IRQHandler                        ; CAN0 Interrupt indicating that errors were detected on the CAN bus
CAN0_Wake_Up_IRQHandler                      ; CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode
CAN0_ORed_0_15_MB_IRQHandler                 ; CAN0 ORed Message buffer (0-15)
CAN0_ORed_16_31_MB_IRQHandler                ; CAN0 ORed Message buffer (16-31)
Reserved99_IRQHandler                            ; Reserved Interrupt 99
Reserved100_IRQHandler                           ; Reserved Interrupt 100
CAN1_ORed_IRQHandler                         ; CAN1 ORed
CAN1_Error_IRQHandler                        ; CAN1 Interrupt indicating that errors were detected on the CAN bus
Reserved103_IRQHandler                           ; Reserved Interrupt 103
CAN1_ORed_0_15_MB_IRQHandler                 ; CAN1 ORed Interrupt for Message buffer (0-15)
CAN1_ORed_16_31_MB_IRQHandler                ; CAN1 ORed Interrupt for Message buffer (16-31)
Reserved106_IRQHandler                           ; Reserved Interrupt 106
Reserved107_IRQHandler                           ; Reserved Interrupt 107
CAN2_ORed_IRQHandler                         ; CAN2 ORed
CAN2_Error_IRQHandler                        ; CAN2 Interrupt indicating that errors were detected on the CAN bus
Reserved110_IRQHandler                           ; Reserved Interrupt 110
CAN2_ORed_0_15_MB_IRQHandler                 ; CAN2 ORed Message buffer (0-15)
Reserved112_IRQHandler                           ; Reserved Interrupt 112
Reserved113_IRQHandler                           ; Reserved Interrupt 113
Reserved114_IRQHandler                           ; Reserved Interrupt 114
CFGTMR0_Ch0_Ch1_IRQHandler                         ; CFGTMR0 Channel 0 and 1 interrupt
CFGTMR0_Ch2_Ch3_IRQHandler                         ; CFGTMR0 Channel 2 and 3 interrupt
CFGTMR0_Ch4_Ch5_IRQHandler                         ; CFGTMR0 Channel 4 and 5 interrupt
CFGTMR0_Ch6_Ch7_IRQHandler                         ; CFGTMR0 Channel 6 and 7 interrupt
CFGTMR0_Fault_IRQHandler                           ; CFGTMR0 Fault interrupt
CFGTMR0_Ovf_Reload_IRQHandler                      ; CFGTMR0 Counter overflow and Reload interrupt
CFGTMR1_Ch0_Ch1_IRQHandler                         ; CFGTMR1 Channel 0 and 1 interrupt
CFGTMR1_Ch2_Ch3_IRQHandler                         ; CFGTMR1 Channel 2 and 3 interrupt
CFGTMR1_Ch4_Ch5_IRQHandler                         ; CFGTMR1 Channel 4 and 5 interrupt
CFGTMR1_Ch6_Ch7_IRQHandler                         ; CFGTMR1 Channel 6 and 7 interrupt
CFGTMR1_Fault_IRQHandler                           ; CFGTMR1 Fault interrupt
CFGTMR1_Ovf_Reload_IRQHandler                      ; CFGTMR1 Counter overflow and Reload interrupt
CFGTMR2_Ch0_Ch1_IRQHandler                         ; CFGTMR2 Channel 0 and 1 interrupt
CFGTMR2_Ch2_Ch3_IRQHandler                         ; CFGTMR2 Channel 2 and 3 interrupt
CFGTMR2_Ch4_Ch5_IRQHandler                         ; CFGTMR2 Channel 4 and 5 interrupt
CFGTMR2_Ch6_Ch7_IRQHandler                         ; CFGTMR2 Channel 6 and 7 interrupt
CFGTMR2_Fault_IRQHandler                           ; CFGTMR2 Fault interrupt
CFGTMR2_Ovf_Reload_IRQHandler                      ; CFGTMR2 Counter overflow and Reload interrupt
CFGTMR3_Ch0_Ch1_IRQHandler                         ; CFGTMR3 Channel 0 and 1 interrupt
CFGTMR3_Ch2_Ch3_IRQHandler                         ; CFGTMR3 Channel 2 and 3 interrupt
CFGTMR3_Ch4_Ch5_IRQHandler                         ; CFGTMR3 Channel 4 and 5 interrupt
CFGTMR3_Ch6_Ch7_IRQHandler                         ; CFGTMR3 Channel 6 and 7 interrupt
CFGTMR3_Fault_IRQHandler                           ; CFGTMR3 Fault interrupt
CFGTMR3_Ovf_Reload_IRQHandler                      ; CFGTMR3 Counter overflow and Reload interrupt
CFGTMR4_Ch0_Ch1_IRQHandler                         ; CFGTMR4 Channel 0 and 1 interrupt
CFGTMR4_Ch2_Ch3_IRQHandler                         ; CFGTMR4 Channel 2 and 3 interrupt
CFGTMR4_Ch4_Ch5_IRQHandler                         ; CFGTMR4 Channel 4 and 5 interrupt
CFGTMR4_Ch6_Ch7_IRQHandler                         ; CFGTMR4 Channel 6 and 7 interrupt
CFGTMR4_Fault_IRQHandler                           ; CFGTMR4 Fault interrupt
CFGTMR4_Ovf_Reload_IRQHandler                      ; CFGTMR4 Counter overflow and Reload interrupt
CFGTMR5_Ch0_Ch1_IRQHandler                         ; CFGTMR5 Channel 0 and 1 interrupt
CFGTMR5_Ch2_Ch3_IRQHandler                         ; CFGTMR5 Channel 2 and 3 interrupt
CFGTMR5_Ch4_Ch5_IRQHandler                         ; CFGTMR5 Channel 4 and 5 interrupt
CFGTMR5_Ch6_Ch7_IRQHandler                         ; CFGTMR5 Channel 6 and 7 interrupt
CFGTMR5_Fault_IRQHandler                           ; CFGTMR5 Fault interrupt
CFGTMR5_Ovf_Reload_IRQHandler                      ; CFGTMR5 Counter overflow and Reload interrupt
                B       .

                ENDP

                ALIGN
                END