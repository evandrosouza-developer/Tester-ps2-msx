#pragma once

//Use Tab width=2

#ifdef __cplusplus
extern "C" {
#endif

#define SCAN_POINTER_SIZE				18
#define DELAY_TO_READ_SIZE			14


#define INIT_SCAN_POINTER				15
#define INIT_DELAY_TO_READ_X_SCAN	6

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
//Force Xint_pin_id to low. portXread in msxmap.cpp will put this in high at each port update: to be possible mesaure real time of reading.
#define Y_0   0x00F20001  // 15859713
#define Y_1   0x00E20011  // 14811153
#define Y_2   0x00D20021  // 13762593
#define Y_3   0x00C20031  // 12714033
#define Y_4   0x00B20041  // 11665473
#define Y_5   0x00A20051  // 10616913
#define Y_6   0x00920061  // 9568353
#define Y_7   0x00820071  // 8519793
#define Y_8   0x00720081  // 7471233
#define Y_9   0x00620091  // 6422673
#define Y_A   0x005200A1  // 5374113
#define Y_B   0x004200B1  // 4325553
#define Y_C   0x003200C1  // 3276993
#define Y_D   0x002200D1  // 2228433
#define Y_E   0x001200E1  // 1179873
#define Y_F   0x000200F1  // 131313


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
