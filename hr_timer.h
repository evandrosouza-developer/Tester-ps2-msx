#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//Definitions of TIM2 stateful machine
#define TIME_CAPTURE	1056	//0x420 //"Normal" state
#define SEND_ST_BIT_2	1057	//0x421
#define SEND_ST_BIT_3	1058	//0x422

void tim_hr_setup(uint32_t);
void tim_usb_tx_setup(void);
void usb_tx_usec(uint32_t, uint16_t);

void delay_usec(uint32_t, uint16_t, void next_step (void));
void delay_qusec(uint32_t, uint16_t, void next_step (void));

void prepares_capture(uint32_t timer_peripheral);

#ifdef __cplusplus
}
#endif
