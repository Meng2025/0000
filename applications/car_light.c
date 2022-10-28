#include "car_light.h"
#include "dr16.h"

/*-------------------------------   大灯和刹车灯   -------------------------------*/
static rt_thread_t car_light = RT_NULL;

static void car_light_thread(void *parameter)
{
    rt_int16_t  ch3 = 1024;
    rt_int16_t  ch3_last = 1024;
    rt_int8_t   s1;

    
    rt_pin_mode(LIGHT_FRONT_1, PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_FRONT_2, PIN_MODE_OUTPUT);
    
    rt_pin_write(LIGHT_FRONT_1, PIN_LOW);
    rt_pin_write(LIGHT_FRONT_2, PIN_LOW);

    rt_pin_mode(LIGHT_BREAK_1, PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_BREAK_2, PIN_MODE_OUTPUT);
    
    rt_pin_write(LIGHT_BREAK_1, PIN_LOW);
    rt_pin_write(LIGHT_BREAK_2, PIN_LOW);
    
    while (1)
    {
        ch3 = dr16.rc.ch3;
        s1  = dr16.rc.s1;
        
        /* 制动亮刹车灯 */
        if (s1 == RC_SW_UP)     /* 左拨杆 OFF，制动 */
        {
            //开
            rt_pin_write(LIGHT_BREAK_1, PIN_HIGH);
            rt_pin_write(LIGHT_BREAK_2, PIN_HIGH);
        }
        else
        {
            /* 减速亮刹车灯 */
            if ( (ch3 > 0) && (ch3 - ch3_last < -80))
            {
                rt_pin_write(LIGHT_BREAK_1, PIN_HIGH);
                rt_pin_write(LIGHT_BREAK_2, PIN_HIGH);
                //开
            }
            else
            {
                rt_pin_write(LIGHT_BREAK_1, PIN_LOW);
                rt_pin_write(LIGHT_BREAK_2, PIN_LOW);
                //关
            }
        }
        ch3_last = ch3;
        
        
        /* 大灯 */
        if (s1 == RC_SW_DOWN)   /* 左拨杆 HL */
        {
            rt_pin_write(LIGHT_FRONT_1, PIN_HIGH);
            rt_pin_write(LIGHT_FRONT_2, PIN_HIGH);             
        }
        else
        {
            rt_pin_write(LIGHT_FRONT_1, PIN_LOW);
            rt_pin_write(LIGHT_FRONT_2, PIN_LOW);              
        }
        rt_thread_mdelay(300);
    }
}

int car_light_start(void)
{
  car_light = rt_thread_create("light",
                              car_light_thread, 
                              RT_NULL,
                              CAR_LIGHT_STACK_SIZE,
                              CAR_LIGHT_PRIORITY, 
                              CAR_LIGHT_TIMESLICE); 

  if (car_light != RT_NULL)
      rt_thread_startup(car_light);
  
  return 0;
}

//MSH_CMD_EXPORT(led_flow_start, led flow start);
INIT_APP_EXPORT(car_light_start);




/*-------------------------------   线程1：转向灯   -------------------------------*/
static rt_thread_t turn_light = RT_NULL;

static void turn_light_thread(void *parameter)
{
    rt_int8_t   s2;
    int l_status = PIN_LOW;
    int r_status = PIN_LOW;

    rt_pin_mode(LIGHT_LEFT_1, PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_LEFT_2, PIN_MODE_OUTPUT);
    
    rt_pin_write(LIGHT_LEFT_1, PIN_LOW);
    rt_pin_write(LIGHT_LEFT_2, PIN_LOW);
    
    rt_pin_mode(LIGHT_RIGHT_1, PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_RIGHT_2, PIN_MODE_OUTPUT);
    
    rt_pin_write(LIGHT_RIGHT_1, PIN_LOW);
    rt_pin_write(LIGHT_RIGHT_2, PIN_LOW);
    while (1)
    {
        s2  = dr16.rc.s2;
        
        l_status = !rt_pin_read(LIGHT_LEFT_1);
        r_status = !rt_pin_read(LIGHT_RIGHT_1);

        if (s2 == RC_SW_UP)     /* 左转向灯 */
        {
            rt_pin_write(LIGHT_LEFT_1,l_status);
            rt_pin_write(LIGHT_LEFT_2,l_status);
        }

        if (s2 == RC_SW_DOWN)   /* 右转向灯 */
        {
            rt_pin_write(LIGHT_RIGHT_1,r_status);
            rt_pin_write(LIGHT_RIGHT_2,r_status);
        }
        
        if (s2 == RC_SW_MID)
        {
            rt_pin_write(LIGHT_RIGHT_1, PIN_LOW);
            rt_pin_write(LIGHT_RIGHT_2,PIN_LOW);            
            rt_pin_write(LIGHT_LEFT_1,PIN_LOW);
            rt_pin_write(LIGHT_LEFT_2,PIN_LOW);        
        }

        rt_thread_mdelay(300);
    }
}

int turn_light_start(void)
{
  turn_light = rt_thread_create("tlight",
                              turn_light_thread, 
                              RT_NULL,
                              CAR_LIGHT_STACK_SIZE,
                              CAR_LIGHT_PRIORITY, 
                              CAR_LIGHT_TIMESLICE); 

  if (turn_light != RT_NULL)
      rt_thread_startup(turn_light);
  
  return 0;
}


INIT_APP_EXPORT(turn_light_start);





