#ifndef msxmap_h
#define msxmap_h

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>

//Use Tab width=2


class msxmap
{
private:

public:
	void msx_interface_setup(void);
	void general_debug_setup(void);
};

	void portXread(void);

#endif
