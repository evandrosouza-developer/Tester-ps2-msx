#pragma once

//Use Tab width=2

#ifdef __cplusplus
extern "C" {
#endif

#define SCAN_POINTER_SIZE				18
#define DELAY_TO_READ_SIZE			14


#define INIT_SCAN_POINTER				15
#define INIT_DELAY_TO_READ_X_SCAN	8

#define X7_port									GPIOB
#define X7_pin_id								GPIO15
#define X7_exti									EXTI15
#define X6_port									GPIOB
#define X6_pin_id								GPIO14
#define X6_exti									EXTI14
#define X5_port									GPIOB
#define X5_pin_id								GPIO13
#define X5_exti									EXTI13
#define X4_port									GPIOB
#define X4_pin_id								GPIO12
#define X4_exti									EXTI12
#define X3_port									GPIOB
#define X3_pin_id								GPIO11
#define X3_exti									EXTI11
#define X2_port									GPIOB
#define X2_pin_id								GPIO10
#define X2_exti									EXTI10
#define X1_port									GPIOB
#define X1_pin_id								GPIO9
#define X1_exti									EXTI9
#define X0_port									GPIOB
#define X0_pin_id								GPIO8
#define X0_exti									EXTI8
#define Y_port									GPIOA
#define Y_pin_id								GPIO0
#define Y3_port									GPIOA
#define Y3_pin_id								GPIO7
#define Y2_port									GPIOA
#define Y2_pin_id								GPIO6
#define Y1_port									GPIOA
#define Y1_pin_id								GPIO5
#define Y0_port									GPIOA
#define Y0_pin_id								GPIO4
#define CAPS_port								GPIOB
#define CAPS_pin_id							GPIO5
#define KANA_port								GPIOB
#define KANA_pin_id							GPIO6
//SERIAL1_port									GPIOA (Pre-defined)
//SERIAL1_TX_pin_id							GPIO9 (Pre-defined)
//SERIAL1_RX_pin_id							GPIO10(Pre-defined)


//Force Y_port_id pin (Sync pin) to high, so the first time slot is a low => Falling transition on the start of frame
#define Y_0   0x00F00001  // 15728641
#define Y_1   0x00E00011  // 14680081
#define Y_2   0x00D00021  // 13631521
#define Y_3   0x00C00031  // 12582961
#define Y_4   0x00B00041  // 11534401
#define Y_5   0x00A00051  // 10485841
#define Y_6   0x00900061  // 9437281
#define Y_7   0x00800071  // 8388721
#define Y_8   0x00700081  // 7340161
#define Y_9   0x00600091  // 6291601
#define Y_A   0x005000A1  // 5243041
#define Y_B   0x004000B1  // 4194481
#define Y_C   0x003000C1  // 3145921
#define Y_D   0x002000D1  // 2097361
#define Y_E   0x001000E1  // 1048801
#define Y_F   0x000000F1  // 241


/*//Force Y_port_id pin (Sync pin) to low, so we have a falling transition on start of each bit
#define Y_0   0x00F10000  // 15794176
#define Y_1   0x00E10010  // 14745616
#define Y_2   0x00D10020  // 13697056
#define Y_3   0x00C10030  // 12648496
#define Y_4   0x00B10040  // 11599936
#define Y_5   0x00A10050  // 10551376
#define Y_6   0x00910060  // 9502816
#define Y_7   0x00810070  // 8454256
#define Y_8   0x00710080  // 7405696
#define Y_9   0x00610090  // 6357136
#define Y_A   0x005100A0  // 5308576
#define Y_B   0x004100B0  // 4260016
#define Y_C   0x003100C0  // 3211456
#define Y_D   0x002100D0  // 2162896
#define Y_E   0x001100E0  // 1114336
#define Y_F   0x000100F0  // 65776
*/


//Para debug
#define Xint_port								GPIOA
#define Xint_pin_id							GPIO1
#define INT_TIM2_port						GPIOB
#define TIM2UIF_pin_id					GPIO0
#define SYSTICK_port						GPIOB
#define SYSTICK_pin_id					GPIO1


#ifdef __cplusplus
}
#endif
