#ifndef __DELAY__
#define __DELAY__


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"
#define delay_ms(A) (vTaskDelay(pdMS_TO_TICKS(A)))
#define delay_us(A)  ets_delay_us(A)

#endif