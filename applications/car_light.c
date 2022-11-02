#include "car_light.h"
#include "sbus.h"

/*-------------------------------   自动刹车灯   -------------------------------*/
#define break_limit     -80

static rt_thread_t break_light = RT_NULL;

static void break_light_thread(void *parameter)
{
    rt_int16_t speed        = SBUS_CH_OFFSET;
    rt_int16_t last_speed   = SBUS_CH_OFFSET;
    rt_int16_t break_light  = SBUS_SW_MID;

    while (1)
    {
        speed       = sbus.lv;
        break_light = sbus.se;
       
        if (break_light == SBUS_SW_UP)     /* SE外拨，熄灭刹车灯 */
        {
            rt_pin_write(LIGHT_BREAK_1, PIN_LOW);
            rt_pin_write(LIGHT_BREAK_2, PIN_LOW);
        }
        else if (break_light == SBUS_SW_MID) /* SE中位，自动刹车灯 */
        {
            /* 停车自动亮刹车灯 */
            if ( speed == SBUS_CH_OFFSET )
            {
                rt_pin_write(LIGHT_BREAK_1, PIN_HIGH);
                rt_pin_write(LIGHT_BREAK_2, PIN_HIGH);
            }
            
            /* 减速自动亮刹车灯 */
            else if ( (speed > SBUS_CH_OFFSET) && (speed - last_speed < break_limit))
            {
                rt_pin_write(LIGHT_BREAK_1, PIN_HIGH);
                rt_pin_write(LIGHT_BREAK_2, PIN_HIGH);
            }
            
            /* 其他情况，关闭刹车灯 */
            else
            {
                rt_pin_write(LIGHT_BREAK_1, PIN_LOW);
                rt_pin_write(LIGHT_BREAK_2, PIN_LOW);
            }
        }
        else
        {
            rt_pin_write(LIGHT_BREAK_1, PIN_HIGH);
            rt_pin_write(LIGHT_BREAK_2, PIN_HIGH);
        }
        last_speed = speed;

        rt_thread_mdelay(300);
    }
}


/*-------------------------------   大灯闪烁   -------------------------------*/
static rt_thread_t front_light = RT_NULL;

static void front_light_thread(void *parameter)
{
    rt_uint16_t   flash = SBUS_CH_MIN;
    rt_uint16_t   light = SBUS_CH_MIN;

    while (1)
    {
        flash = sbus.sh;
        light = sbus.sf;
        
        /* 会车闪灯三次 */
        if (flash == SBUS_CH_MAX)     
        {
            rt_pin_write(LIGHT_FRONT_1, PIN_HIGH);
            rt_pin_write(LIGHT_FRONT_2, PIN_HIGH);
            
            rt_thread_mdelay(150);
            
            rt_pin_write(LIGHT_FRONT_1, PIN_LOW);
            rt_pin_write(LIGHT_FRONT_2, PIN_LOW);
            
            rt_thread_mdelay(100);
            
            rt_pin_write(LIGHT_FRONT_1, PIN_HIGH);
            rt_pin_write(LIGHT_FRONT_2, PIN_HIGH);
            
            rt_thread_mdelay(150);
            
            rt_pin_write(LIGHT_FRONT_1, PIN_LOW);
            rt_pin_write(LIGHT_FRONT_2, PIN_LOW);
            
            rt_thread_mdelay(100);
            
            rt_pin_write(LIGHT_FRONT_1, PIN_HIGH);
            rt_pin_write(LIGHT_FRONT_2, PIN_HIGH);
            
            rt_thread_mdelay(150);
            
            rt_pin_write(LIGHT_FRONT_1, PIN_LOW);
            rt_pin_write(LIGHT_FRONT_2, PIN_LOW); 

            rt_thread_mdelay(150);
        }
        
        if (light == SBUS_SW_DOWN)
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



/*-------------------------------   转向灯闪烁控制   -------------------------------*/
#define bandlimit   450    //自动转向灯的灵敏度

static rt_thread_t turn_light = RT_NULL;

static void turn_light_thread(void *parameter)
{
    rt_int16_t direct = SBUS_CH_OFFSET;
    rt_int16_t turn = SBUS_SW_MID;
    int l_status = PIN_LOW;
    int r_status = PIN_LOW;


    while (1)
    {
        turn = sbus.sg;
        direct = sbus.rh;
        
        l_status = !rt_pin_read(LIGHT_LEFT_1);
        r_status = !rt_pin_read(LIGHT_RIGHT_1);

        /* 手自一体转向灯 */
        if ( (turn == SBUS_SW_UP) || (direct < SBUS_CH_OFFSET - bandlimit))
        {
            rt_pin_write(LIGHT_LEFT_1, l_status);
            rt_pin_write(LIGHT_LEFT_2, l_status);
            
            rt_pin_write(LIGHT_RIGHT_1, PIN_LOW);
            rt_pin_write(LIGHT_RIGHT_2, PIN_LOW);         
        }

        else if ( (turn == SBUS_SW_DOWN) || (direct > SBUS_CH_OFFSET + bandlimit))
        {
            rt_pin_write(LIGHT_RIGHT_1,r_status);
            rt_pin_write(LIGHT_RIGHT_2,r_status);
            
            rt_pin_write(LIGHT_LEFT_1, PIN_LOW);
            rt_pin_write(LIGHT_LEFT_2, PIN_LOW);
        }

        else
        {
            rt_pin_write(LIGHT_RIGHT_1, PIN_LOW);
            rt_pin_write(LIGHT_RIGHT_2,PIN_LOW);            
            rt_pin_write(LIGHT_LEFT_1,PIN_LOW);
            rt_pin_write(LIGHT_LEFT_2,PIN_LOW);        
        }

        rt_thread_mdelay(300);
    }
}


/**/
static rt_thread_t car_light = RT_NULL;

static void car_light_thread(void *parameter)
{
    /* 灯引脚控制 */
    rt_pin_mode(LIGHT_FRONT_1, PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_FRONT_2, PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_BREAK_1, PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_BREAK_2, PIN_MODE_OUTPUT);  
    rt_pin_mode(LIGHT_LEFT_1,  PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_LEFT_2,  PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_RIGHT_1, PIN_MODE_OUTPUT);
    rt_pin_mode(LIGHT_RIGHT_2, PIN_MODE_OUTPUT);
    

    rt_pin_write(LIGHT_FRONT_1, PIN_HIGH);
    rt_pin_write(LIGHT_FRONT_2, PIN_HIGH);
    rt_pin_write(LIGHT_BREAK_1, PIN_HIGH);
    rt_pin_write(LIGHT_BREAK_2, PIN_HIGH);    
    rt_pin_write(LIGHT_LEFT_1,  PIN_HIGH);
    rt_pin_write(LIGHT_LEFT_2,  PIN_HIGH);    
    rt_pin_write(LIGHT_RIGHT_1, PIN_HIGH);
    rt_pin_write(LIGHT_RIGHT_2, PIN_HIGH);
    
    rt_thread_mdelay(3000);

    rt_pin_write(LIGHT_FRONT_1, PIN_LOW);
    rt_pin_write(LIGHT_FRONT_2, PIN_LOW);
    rt_pin_write(LIGHT_BREAK_1, PIN_LOW);
    rt_pin_write(LIGHT_BREAK_2, PIN_LOW);    
    rt_pin_write(LIGHT_LEFT_1,  PIN_LOW);
    rt_pin_write(LIGHT_LEFT_2,  PIN_LOW);    
    rt_pin_write(LIGHT_RIGHT_1, PIN_LOW);
    rt_pin_write(LIGHT_RIGHT_2, PIN_LOW);
    
    
    break_light = rt_thread_create("blight",
                                  break_light_thread, 
                                  RT_NULL,
                                  CAR_LIGHT_STACK_SIZE,
                                  CAR_LIGHT_PRIORITY, 
                                  CAR_LIGHT_TIMESLICE); 

    if (break_light != RT_NULL)
        rt_thread_startup(break_light);
    
    
    front_light = rt_thread_create("flight",
                              front_light_thread, 
                              RT_NULL,
                              CAR_LIGHT_STACK_SIZE,
                              CAR_LIGHT_PRIORITY, 
                              CAR_LIGHT_TIMESLICE); 

    if (front_light != RT_NULL)
        rt_thread_startup(front_light);

    turn_light = rt_thread_create("tlight",
                              turn_light_thread, 
                              RT_NULL,
                              CAR_LIGHT_STACK_SIZE,
                              CAR_LIGHT_PRIORITY, 
                              CAR_LIGHT_TIMESLICE); 

    if (turn_light != RT_NULL)
        rt_thread_startup(turn_light);
}

int car_light_start(void)
{
  car_light = rt_thread_create("carlight",
                              car_light_thread, 
                              RT_NULL,
                              CAR_LIGHT_STACK_SIZE,
                              CAR_LIGHT_PRIORITY, 
                              CAR_LIGHT_TIMESLICE); 

  if (car_light != RT_NULL)
      rt_thread_startup(car_light);
  
  return 0;
}

INIT_APP_EXPORT(car_light_start);


