#ifndef __DIGITAL__
#define __DIGITAL__
#include "driver/gpio.h"

#define HIGH 1
#define LOW 0

#define INPUT GPIO_MODE_INPUT
#define OUTPUT GPIO_MODE_OUTPUT
#define INPUT_PULLUP GPIO_PULLUP_ONLY

#define TRUE 1
#define FALSE 0

class DIGITAL {
	private:
		int debug=0;
	public:
		void pinMode ( gpio_num_t gpio_num, gpio_mode_t mode);
		int digitalRead (gpio_num_t gpio_num);
		void digitalWrite(gpio_num_t gpio_num, uint32_t level);
		void setDebug(int v);
};

 extern   DIGITAL digital;

#endif
