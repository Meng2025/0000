#include "power.h"

#include "sbus.h"


/*-------------------------------   线程1：流水灯   -------------------------------*/
static rt_thread_t power = RT_NULL;

static void power_thread(void *parameter)
{
    rt_uint16_t ctrl = SBUS_SW_MID;
    rt_pin_mode(POWER_EN_PIN1, PIN_MODE_OUTPUT);
    
    while (1)
    {
        ctrl = sbus.sd;
        if (ctrl == SBUS_SW_MID)
        {
            rt_pin_write(POWER_EN_PIN1, PIN_LOW);
        }
        else
        {
            rt_pin_write(POWER_EN_PIN1, PIN_HIGH);
        }
        rt_thread_mdelay(5000);
    }
}

int power_start(void)
{
  power = rt_thread_create("power",
                              power_thread, 
                              RT_NULL,
                              POWER_STACK_SIZE,
                              POWER_PRIORITY, 
                              POWER_TIMESLICE); 

  if (power != RT_NULL)
      rt_thread_startup(power);
  
  return 0;
}


INIT_APP_EXPORT(power_start);







