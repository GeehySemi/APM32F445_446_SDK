;/*!
; * @file        startup_apm32f445.s
; *
; * @brief       IAR Compiler Collection Startup File
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

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvector:CODE:ROOT(2)

        EXTERN  SystemInit
        EXTERN  bss_data_init
        EXTERN  main
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler                                   ;NMI Handler
        DCD     HardFault_Handler                             ;Hard Fault Handler
        DCD     MemManage_Handler                             ;MPU Fault Handler
        DCD     BusFault_Handler                              ;Bus Fault Handler
        DCD     UsageFault_Handler                            ;Usage Fault Handler
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     SVC_Handler                                   ;SVCall Handler
        DCD     DebugMon_Handler                              ;Debug Monitor Handler
        DCD     0                                             ;Reserved
        DCD     PendSV_Handler                                ;PendSV Handler
        DCD     SysTick_Handler                               ;SysTick Handler

        ; External Interrupts
        DCD   DMA0_IRQHandler                                 ; DMA channel 0 transfer complete
        DCD   DMA1_IRQHandler                                 ; DMA channel 1 transfer complete
        DCD   DMA2_IRQHandler                                 ; DMA channel 2 transfer complete
        DCD   DMA3_IRQHandler                                 ; DMA channel 3 transfer complete
        DCD   DMA4_IRQHandler                                 ; DMA channel 4 transfer complete
        DCD   DMA5_IRQHandler                                 ; DMA channel 5 transfer complete
        DCD   DMA6_IRQHandler                                 ; DMA channel 6 transfer complete
        DCD   DMA7_IRQHandler                                 ; DMA channel 7 transfer complete
        DCD   DMA8_IRQHandler                                 ; DMA channel 8 transfer complete
        DCD   DMA9_IRQHandler                                 ; DMA channel 9 transfer complete
        DCD   DMA10_IRQHandler                                ; DMA channel 10 transfer complete
        DCD   DMA11_IRQHandler                                ; DMA channel 11 transfer complete
        DCD   DMA12_IRQHandler                                ; DMA channel 12 transfer complete
        DCD   DMA13_IRQHandler                                ; DMA channel 13 transfer complete
        DCD   DMA14_IRQHandler                                ; DMA channel 14 transfer complete
        DCD   DMA15_IRQHandler                                ; DMA channel 15 transfer complete
        DCD   DMA_Error_IRQHandler                            ; DMA error interrupt channels 0-15
        DCD   MC_IRQHandler                                   ; FPU sources
        DCD   FLASH_IRQHandler                                ; FLASH Command complete
        DCD   Read_Collision_IRQHandler                       ; FLASH Read collision
        DCD   LVD_LVW_IRQHandler                              ; PMU Low voltage detect interrupt
        DCD   FLASH_Fault_IRQHandler                          ; FLASH Double bit fault detect
        DCD   WDT_EWDT_IRQHandler                             ; Single interrupt vector for WDT and EWDT
        DCD   RMU_IRQHandler                                  ; RMU Asynchronous Interrupt
        DCD   LPI2C_Master_IRQHandler                         ; LPI2C Master Interrupt
        DCD   LPI2C_Slave_IRQHandler                          ; LPI2C Slave Interrupt
        DCD   LPSPI0_IRQHandler                               ; LPSPI0 Interrupt
        DCD   LPSPI1_IRQHandler                               ; LPSPI1 Interrupt
        DCD   LPSPI2_IRQHandler                               ; LPSPI2 Interrupt
        DCD   Reserved45_IRQHandler                           ; Reserved Interrupt 45
        DCD   Reserved46_IRQHandler                           ; Reserved Interrupt 46
        DCD   LPUART0_RxTx_IRQHandler                         ; LPUART0 Transmit / Receive Interrupt
        DCD   Reserved48_IRQHandler                           ; Reserved Interrupt 48
        DCD   LPUART1_RxTx_IRQHandler                         ; LPUART1 Transmit / Receive  Interrupt
        DCD   Reserved50_IRQHandler                           ; Reserved Interrupt 50
        DCD   LPUART2_RxTx_IRQHandler                         ; LPUART2 Transmit / Receive  Interrupt
        DCD   Reserved52_IRQHandler                           ; Reserved Interrupt 52
        DCD   Reserved53_IRQHandler                           ; Reserved Interrupt 53
        DCD   Reserved54_IRQHandler                           ; Reserved Interrupt 54
        DCD   ADC0_IRQHandler                                 ; ADC0 interrupt request.
        DCD   ADC1_IRQHandler                                 ; ADC1 interrupt request.
        DCD   COMP_IRQHandler                                 ; COMP interrupt request
        DCD   Reserved58_IRQHandler                           ; Reserved Interrupt 58
        DCD   Reserved59_IRQHandler                           ; Reserved Interrupt 59
        DCD   EREP_single_fault_IRQHandler                    ; EREP single bit error correction
        DCD   EREP_double_fault_IRQHandler                    ; EREP double bit error non-correctable
        DCD   RTC_IRQHandler                                  ; RTC alarm interrupt
        DCD   RTC_Seconds_IRQHandler                          ; RTC seconds interrupt
        DCD   LPITMR_Ch0_IRQHandler                           ; LPITMR channel 0 overflow interrupt
        DCD   LPITMR_Ch1_IRQHandler                           ; LPITMR channel 1 overflow interrupt
        DCD   LPITMR_Ch2_IRQHandler                           ; LPITMR channel 2 overflow interrupt
        DCD   LPITMR_Ch3_IRQHandler                           ; LPITMR channel 3 overflow interrupt
        DCD   PDU0_IRQHandler                                 ; PDU0 interrupt
        DCD   Reserved69_IRQHandler                           ; Reserved Interrupt 69
        DCD   Reserved70_IRQHandler                           ; Reserved Interrupt 70
        DCD   Reserved71_IRQHandler                           ; Reserved Interrupt 71
        DCD   Reserved72_IRQHandler                           ; Reserved Interrupt 72
        DCD   SCG_IRQHandler                                  ; SCG bus interrupt request
        DCD   LPTMR_IRQHandler                                ; LPTMR interrupt request
        DCD   PMA_IRQHandler                                  ; PM A pin detect interrupt
        DCD   PMB_IRQHandler                                  ; PM B pin detect interrupt
        DCD   PMC_IRQHandler                                  ; PM C pin detect interrupt
        DCD   PMD_IRQHandler                                  ; PM D pin detect interrupt
        DCD   PME_IRQHandler                                  ; PM E pin detect interrupt
        DCD   SWI_IRQHandler                                  ; Software interrupt
        DCD   Reserved81_IRQHandler                           ; Reserved Interrupt 81
        DCD   Reserved82_IRQHandler                           ; Reserved Interrupt 82
        DCD   Reserved83_IRQHandler                           ; Reserved Interrupt 83
        DCD   PDU1_IRQHandler                                 ; PDU1 interrupt
        DCD   CFGIO_IRQHandler                              ; CFGIO Interrupt
        DCD   Reserved86_IRQHandler                           ; Reserved Interrupt 86
        DCD   Reserved87_IRQHandler                           ; Reserved Interrupt 87
        DCD   Reserved88_IRQHandler                           ; Reserved Interrupt 88
        DCD   Reserved89_IRQHandler                           ; Reserved Interrupt 89
        DCD   Reserved90_IRQHandler                           ; Reserved Interrupt 90
        DCD   Reserved91_IRQHandler                           ; Reserved Interrupt 91
        DCD   Reserved92_IRQHandler                           ; Reserved Interrupt 92
        DCD   Reserved93_IRQHandler                           ; Reserved Interrupt 93
        DCD   CAN0_ORed_IRQHandler                        ; CAN0 ORed
        DCD   CAN0_Error_IRQHandler                       ; CAN0 Interrupt indicating that errors were detected on the CAN bus
        DCD   CAN0_Wake_Up_IRQHandler                     ; CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode
        DCD   CAN0_ORed_0_15_MB_IRQHandler                ; CAN0 ORed Message buffer (0-15)
        DCD   CAN0_ORed_16_31_MB_IRQHandler               ; CAN0 ORed Message buffer (16-31)
        DCD   Reserved99_IRQHandler                           ; Reserved Interrupt 99
        DCD   Reserved100_IRQHandler                          ; Reserved Interrupt 100
        DCD   CAN1_ORed_IRQHandler                        ; CAN1 ORed
        DCD   CAN1_Error_IRQHandler                       ; CAN1 Interrupt indicating that errors were detected on the CAN bus
        DCD   Reserved103_IRQHandler                          ; Reserved Interrupt 103
        DCD   CAN1_ORed_0_15_MB_IRQHandler                ; CAN1 ORed Interrupt for Message buffer (0-15)
        DCD   Reserved105_IRQHandler                          ; Reserved Interrupt 105
        DCD   Reserved106_IRQHandler                          ; Reserved Interrupt 106
        DCD   Reserved107_IRQHandler                          ; Reserved Interrupt 107
        DCD   CAN2_ORed_IRQHandler                        ; CAN2 ORed
        DCD   CAN2_Error_IRQHandler                       ; CAN2 Interrupt indicating that errors were detected on the CAN bus
        DCD   Reserved110_IRQHandler                          ; Reserved Interrupt 110
        DCD   CAN2_ORed_0_15_MB_IRQHandler                ; CAN2 ORed Message buffer (0-15)
        DCD   Reserved112_IRQHandler                          ; Reserved Interrupt 112
        DCD   Reserved113_IRQHandler                          ; Reserved Interrupt 113
        DCD   Reserved114_IRQHandler                          ; Reserved Interrupt 114
        DCD   CFGTMR0_Ch0_Ch1_IRQHandler                        ; CFGTMR0 Channel 0 and 1 interrupt
        DCD   CFGTMR0_Ch2_Ch3_IRQHandler                        ; CFGTMR0 Channel 2 and 3 interrupt
        DCD   CFGTMR0_Ch4_Ch5_IRQHandler                        ; CFGTMR0 Channel 4 and 5 interrupt
        DCD   CFGTMR0_Ch6_Ch7_IRQHandler                        ; CFGTMR0 Channel 6 and 7 interrupt
        DCD   CFGTMR0_Fault_IRQHandler                          ; CFGTMR0 Fault interrupt
        DCD   CFGTMR0_Ovf_Reload_IRQHandler                     ; CFGTMR0 Counter overflow and Reload interrupt
        DCD   CFGTMR1_Ch0_Ch1_IRQHandler                        ; CFGTMR1 Channel 0 and 1 interrupt
        DCD   CFGTMR1_Ch2_Ch3_IRQHandler                        ; CFGTMR1 Channel 2 and 3 interrupt
        DCD   CFGTMR1_Ch4_Ch5_IRQHandler                        ; CFGTMR1 Channel 4 and 5 interrupt
        DCD   CFGTMR1_Ch6_Ch7_IRQHandler                        ; CFGTMR1 Channel 6 and 7 interrupt
        DCD   CFGTMR1_Fault_IRQHandler                          ; CFGTMR1 Fault interrupt
        DCD   CFGTMR1_Ovf_Reload_IRQHandler                     ; CFGTMR1 Counter overflow and Reload interrupt
        DCD   CFGTMR2_Ch0_Ch1_IRQHandler                        ; CFGTMR2 Channel 0 and 1 interrupt
        DCD   CFGTMR2_Ch2_Ch3_IRQHandler                        ; CFGTMR2 Channel 2 and 3 interrupt
        DCD   CFGTMR2_Ch4_Ch5_IRQHandler                        ; CFGTMR2 Channel 4 and 5 interrupt
        DCD   CFGTMR2_Ch6_Ch7_IRQHandler                        ; CFGTMR2 Channel 6 and 7 interrupt
        DCD   CFGTMR2_Fault_IRQHandler                          ; CFGTMR2 Fault interrupt
        DCD   CFGTMR2_Ovf_Reload_IRQHandler                     ; CFGTMR2 Counter overflow and Reload interrupt
        DCD   CFGTMR3_Ch0_Ch1_IRQHandler                        ; CFGTMR3 Channel 0 and 1 interrupt
        DCD   CFGTMR3_Ch2_Ch3_IRQHandler                        ; CFGTMR3 Channel 2 and 3 interrupt
        DCD   CFGTMR3_Ch4_Ch5_IRQHandler                        ; CFGTMR3 Channel 4 and 5 interrupt
        DCD   CFGTMR3_Ch6_Ch7_IRQHandler                        ; CFGTMR3 Channel 6 and 7 interrupt
        DCD   CFGTMR3_Fault_IRQHandler                          ; CFGTMR3 Fault interrupt
        DCD   CFGTMR3_Ovf_Reload_IRQHandler                     ; CFGTMR3 Counter overflow and Reload interrupt
        DCD   DefaultISR                                      ; DefaultISR
__Vectors_End

        SECTION FlashConfig:CODE
__FlashConfig
        DCD 0xFFFFFFFF    ; BackDoorKey0:BackDoorKey1:BackDoorKey2:BackDoorKey3
        DCD 0xFFFFFFFF    ; BackDoorKey4:BackDoorKey5:BackDoorKey6:BackDoorKey7
        DCD 0xFFFFFFFF    ; PFPRO0:PFPRO1:PFPRO2:PFPRO3
        DCD 0xFFFF7FFE    ; DFPRO:EEPRO:NOPT:SEC(0xFE = unsecured)
__FlashConfig_End

;; Default interrupt handlers.
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        CPSID   I
        MOV     R1, #0x0
        MOV     R2, #0x0
        MOV     R3, #0x0
        MOV     R4, #0x0
        MOV     R5, #0x0
        MOV     R6, #0x0
        MOV     R7, #0x0
        MOV     R8, #0x0
        MOV     R9, #0x0
        MOV     R10, #0x0
        MOV     R11, #0x0
        MOV     R12, #0x0

#ifdef START_FROM_FLASH
        IMPORT __RAM_START
        IMPORT __RAM_END

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

        ; Set Top of Stack
        LDR     R0, =__vector_table
        LDR     R1, [R0]
        MSR     MSP, R1

        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =bss_data_init
        BLX     R0
        CPSIE   I
        LDR     R0, =main
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B .

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B .

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B .

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B .

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B .

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B .

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B .

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B .

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B .

        PUBWEAK  DMA0_IRQHandler
        PUBWEAK  DMA1_IRQHandler
        PUBWEAK  DMA2_IRQHandler
        PUBWEAK  DMA3_IRQHandler
        PUBWEAK  DMA4_IRQHandler
        PUBWEAK  DMA5_IRQHandler
        PUBWEAK  DMA6_IRQHandler
        PUBWEAK  DMA7_IRQHandler
        PUBWEAK  DMA8_IRQHandler
        PUBWEAK  DMA9_IRQHandler
        PUBWEAK  DMA10_IRQHandler
        PUBWEAK  DMA11_IRQHandler
        PUBWEAK  DMA12_IRQHandler
        PUBWEAK  DMA13_IRQHandler
        PUBWEAK  DMA14_IRQHandler
        PUBWEAK  DMA15_IRQHandler
        PUBWEAK  DMA_Error_IRQHandler
        PUBWEAK  MC_IRQHandler
        PUBWEAK  FLASH_IRQHandler
        PUBWEAK  Read_Collision_IRQHandler
        PUBWEAK  LVD_LVW_IRQHandler
        PUBWEAK  FLASH_Fault_IRQHandler
        PUBWEAK  WDT_EWDT_IRQHandler
        PUBWEAK  RMU_IRQHandler
        PUBWEAK  LPI2C_Master_IRQHandler
        PUBWEAK  LPI2C_Slave_IRQHandler
        PUBWEAK  LPSPI0_IRQHandler
        PUBWEAK  LPSPI1_IRQHandler
        PUBWEAK  LPSPI2_IRQHandler
        PUBWEAK  Reserved45_IRQHandler
        PUBWEAK  Reserved46_IRQHandler
        PUBWEAK  LPUART0_RxTx_IRQHandler
        PUBWEAK  Reserved48_IRQHandler
        PUBWEAK  LPUART1_RxTx_IRQHandler
        PUBWEAK  Reserved50_IRQHandler
        PUBWEAK  LPUART2_RxTx_IRQHandler
        PUBWEAK  Reserved52_IRQHandler
        PUBWEAK  Reserved53_IRQHandler
        PUBWEAK  Reserved54_IRQHandler
        PUBWEAK  ADC0_IRQHandler
        PUBWEAK  ADC1_IRQHandler
        PUBWEAK  COMP_IRQHandler
        PUBWEAK  Reserved58_IRQHandler
        PUBWEAK  Reserved59_IRQHandler
        PUBWEAK  EREP_single_fault_IRQHandler
        PUBWEAK  EREP_double_fault_IRQHandler
        PUBWEAK  RTC_IRQHandler
        PUBWEAK  RTC_Seconds_IRQHandler
        PUBWEAK  LPITMR_Ch0_IRQHandler
        PUBWEAK  LPITMR_Ch1_IRQHandler
        PUBWEAK  LPITMR_Ch2_IRQHandler
        PUBWEAK  LPITMR_Ch3_IRQHandler
        PUBWEAK  PDU0_IRQHandler
        PUBWEAK  Reserved69_IRQHandler
        PUBWEAK  Reserved70_IRQHandler
        PUBWEAK  Reserved71_IRQHandler
        PUBWEAK  Reserved72_IRQHandler
        PUBWEAK  SCG_IRQHandler
        PUBWEAK  LPTMR_IRQHandler
        PUBWEAK  PMA_IRQHandler
        PUBWEAK  PMB_IRQHandler
        PUBWEAK  PMC_IRQHandler
        PUBWEAK  PMD_IRQHandler
        PUBWEAK  PME_IRQHandler
        PUBWEAK  SWI_IRQHandler
        PUBWEAK  Reserved81_IRQHandler
        PUBWEAK  Reserved82_IRQHandler
        PUBWEAK  Reserved83_IRQHandler
        PUBWEAK  PDU1_IRQHandler
        PUBWEAK  CFGIO_IRQHandler
        PUBWEAK  Reserved86_IRQHandler
        PUBWEAK  Reserved87_IRQHandler
        PUBWEAK  Reserved88_IRQHandler
        PUBWEAK  Reserved89_IRQHandler
        PUBWEAK  Reserved90_IRQHandler
        PUBWEAK  Reserved91_IRQHandler
        PUBWEAK  Reserved92_IRQHandler
        PUBWEAK  Reserved93_IRQHandler
        PUBWEAK  CAN0_ORed_IRQHandler
        PUBWEAK  CAN0_Error_IRQHandler
        PUBWEAK  CAN0_Wake_Up_IRQHandler
        PUBWEAK  CAN0_ORed_0_15_MB_IRQHandler
        PUBWEAK  CAN0_ORed_16_31_MB_IRQHandler
        PUBWEAK  Reserved99_IRQHandler
        PUBWEAK  Reserved100_IRQHandler
        PUBWEAK  CAN1_ORed_IRQHandler
        PUBWEAK  CAN1_Error_IRQHandler
        PUBWEAK  Reserved103_IRQHandler
        PUBWEAK  CAN1_ORed_0_15_MB_IRQHandler
        PUBWEAK  Reserved105_IRQHandler
        PUBWEAK  Reserved106_IRQHandler
        PUBWEAK  Reserved107_IRQHandler
        PUBWEAK  CAN2_ORed_IRQHandler
        PUBWEAK  CAN2_Error_IRQHandler
        PUBWEAK  Reserved110_IRQHandler
        PUBWEAK  CAN2_ORed_0_15_MB_IRQHandler
        PUBWEAK  Reserved112_IRQHandler
        PUBWEAK  Reserved113_IRQHandler
        PUBWEAK  Reserved114_IRQHandler
        PUBWEAK  CFGTMR0_Ch0_Ch1_IRQHandler
        PUBWEAK  CFGTMR0_Ch2_Ch3_IRQHandler
        PUBWEAK  CFGTMR0_Ch4_Ch5_IRQHandler
        PUBWEAK  CFGTMR0_Ch6_Ch7_IRQHandler
        PUBWEAK  CFGTMR0_Fault_IRQHandler
        PUBWEAK  CFGTMR0_Ovf_Reload_IRQHandler
        PUBWEAK  CFGTMR1_Ch0_Ch1_IRQHandler
        PUBWEAK  CFGTMR1_Ch2_Ch3_IRQHandler
        PUBWEAK  CFGTMR1_Ch4_Ch5_IRQHandler
        PUBWEAK  CFGTMR1_Ch6_Ch7_IRQHandler
        PUBWEAK  CFGTMR1_Fault_IRQHandler
        PUBWEAK  CFGTMR1_Ovf_Reload_IRQHandler
        PUBWEAK  CFGTMR2_Ch0_Ch1_IRQHandler
        PUBWEAK  CFGTMR2_Ch2_Ch3_IRQHandler
        PUBWEAK  CFGTMR2_Ch4_Ch5_IRQHandler
        PUBWEAK  CFGTMR2_Ch6_Ch7_IRQHandler
        PUBWEAK  CFGTMR2_Fault_IRQHandler
        PUBWEAK  CFGTMR2_Ovf_Reload_IRQHandler
        PUBWEAK  CFGTMR3_Ch0_Ch1_IRQHandler
        PUBWEAK  CFGTMR3_Ch2_Ch3_IRQHandler
        PUBWEAK  CFGTMR3_Ch4_Ch5_IRQHandler
        PUBWEAK  CFGTMR3_Ch6_Ch7_IRQHandler
        PUBWEAK  CFGTMR3_Fault_IRQHandler
        PUBWEAK  CFGTMR3_Ovf_Reload_IRQHandler
        PUBWEAK DefaultISR
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA0_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA3_IRQHandler
DMA4_IRQHandler
DMA5_IRQHandler
DMA6_IRQHandler
DMA7_IRQHandler
DMA8_IRQHandler
DMA9_IRQHandler
DMA10_IRQHandler
DMA11_IRQHandler
DMA12_IRQHandler
DMA13_IRQHandler
DMA14_IRQHandler
DMA15_IRQHandler
DMA_Error_IRQHandler
MC_IRQHandler
FLASH_IRQHandler
Read_Collision_IRQHandler
LVD_LVW_IRQHandler
FLASH_Fault_IRQHandler
WDT_EWDT_IRQHandler
RMU_IRQHandler
LPI2C_Master_IRQHandler
LPI2C_Slave_IRQHandler
LPSPI0_IRQHandler
LPSPI1_IRQHandler
LPSPI2_IRQHandler
Reserved45_IRQHandler
Reserved46_IRQHandler
LPUART0_RxTx_IRQHandler
Reserved48_IRQHandler
LPUART1_RxTx_IRQHandler
Reserved50_IRQHandler
LPUART2_RxTx_IRQHandler
Reserved52_IRQHandler
Reserved53_IRQHandler
Reserved54_IRQHandler
ADC0_IRQHandler
ADC1_IRQHandler
COMP_IRQHandler
Reserved58_IRQHandler
Reserved59_IRQHandler
EREP_single_fault_IRQHandler
EREP_double_fault_IRQHandler
RTC_IRQHandler
RTC_Seconds_IRQHandler
LPITMR_Ch0_IRQHandler
LPITMR_Ch1_IRQHandler
LPITMR_Ch2_IRQHandler
LPITMR_Ch3_IRQHandler
PDU0_IRQHandler
Reserved69_IRQHandler
Reserved70_IRQHandler
Reserved71_IRQHandler
Reserved72_IRQHandler
SCG_IRQHandler
LPTMR_IRQHandler
PMA_IRQHandler
PMB_IRQHandler
PMC_IRQHandler
PMD_IRQHandler
PME_IRQHandler
SWI_IRQHandler
Reserved81_IRQHandler
Reserved82_IRQHandler
Reserved83_IRQHandler
PDU1_IRQHandler
CFGIO_IRQHandler
Reserved86_IRQHandler
Reserved87_IRQHandler
Reserved88_IRQHandler
Reserved89_IRQHandler
Reserved90_IRQHandler
Reserved91_IRQHandler
Reserved92_IRQHandler
Reserved93_IRQHandler
CAN0_ORed_IRQHandler
CAN0_Error_IRQHandler
CAN0_Wake_Up_IRQHandler
CAN0_ORed_0_15_MB_IRQHandler
CAN0_ORed_16_31_MB_IRQHandler
Reserved99_IRQHandler
Reserved100_IRQHandler
CAN1_ORed_IRQHandler
CAN1_Error_IRQHandler
Reserved103_IRQHandler
CAN1_ORed_0_15_MB_IRQHandler
Reserved105_IRQHandler
Reserved106_IRQHandler
Reserved107_IRQHandler
CAN2_ORed_IRQHandler
CAN2_Error_IRQHandler
Reserved110_IRQHandler
CAN2_ORed_0_15_MB_IRQHandler
Reserved112_IRQHandler
Reserved113_IRQHandler
Reserved114_IRQHandler
CFGTMR0_Ch0_Ch1_IRQHandler
CFGTMR0_Ch2_Ch3_IRQHandler
CFGTMR0_Ch4_Ch5_IRQHandler
CFGTMR0_Ch6_Ch7_IRQHandler
CFGTMR0_Fault_IRQHandler
CFGTMR0_Ovf_Reload_IRQHandler
CFGTMR1_Ch0_Ch1_IRQHandler
CFGTMR1_Ch2_Ch3_IRQHandler
CFGTMR1_Ch4_Ch5_IRQHandler
CFGTMR1_Ch6_Ch7_IRQHandler
CFGTMR1_Fault_IRQHandler
CFGTMR1_Ovf_Reload_IRQHandler
CFGTMR2_Ch0_Ch1_IRQHandler
CFGTMR2_Ch2_Ch3_IRQHandler
CFGTMR2_Ch4_Ch5_IRQHandler
CFGTMR2_Ch6_Ch7_IRQHandler
CFGTMR2_Fault_IRQHandler
CFGTMR2_Ovf_Reload_IRQHandler
CFGTMR3_Ch0_Ch1_IRQHandler
CFGTMR3_Ch2_Ch3_IRQHandler
CFGTMR3_Ch4_Ch5_IRQHandler
CFGTMR3_Ch6_Ch7_IRQHandler
CFGTMR3_Fault_IRQHandler
CFGTMR3_Ovf_Reload_IRQHandler
DefaultISR
        B DefaultISR

        END
