#include <stdlib.h>
#include "motor_pwm.h"
#include "dbus.h"



/*----------------------   pwm周期占空比初始化配置   --------------------------*/
int motor_pwm_init(void)
{
    rt_err_t result = RT_EOK;

    struct rt_device_pwm *pwm_dev;
    
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_MOTOR_DEV);
    
    result = rt_pwm_set(pwm_dev, PWM_MOTOR_CH, 10000, 0);
    
    if (result != RT_EOK)
        rt_kprintf("%s channel %d config error.\n", PWM_MOTOR_DEV, PWM_MOTOR_CH);
       
    result = rt_pwm_enable(pwm_dev, PWM_MOTOR_CH);
    
    if (result != RT_EOK)
        rt_kprintf("%s channel enable error.\n", PWM_MOTOR_DEV);
    return 0;
}

INIT_APP_EXPORT(motor_pwm_init);


/*----------------------   pwm舵机控制线程   --------------------------*/


static rt_thread_t motor_pwm_thread = RT_NULL;


/* 线程 1 的入口函数 */
static void motor_pwm_thread_entry(void *parameter)
{
    rt_int32_t speed;
    rt_int32_t mode1 = RC_SW_UP;
   
    struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */
    
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_MOTOR_DEV);
    
    rt_pin_mode(MOTOR_A1, PIN_MODE_OUTPUT);
    rt_pin_mode(MOTOR_A2, PIN_MODE_OUTPUT);

    rt_pin_write(MOTOR_A1, PIN_HIGH);
    rt_pin_write(MOTOR_A2, PIN_HIGH);
    
    while (1)
    {
        speed = dbus.lv;
        mode1 = dbus.sl;
         
        if (mode1 == RC_SW_UP) /* 左手 OFF 制动模式 */
        {
            speed = 0; 
            rt_pin_write(MOTOR_A1, PIN_LOW);
            rt_pin_write(MOTOR_A2, PIN_LOW);
        }
        else 
        {
            speed = (speed - 1024) * 200 /660; /* 摇杆量转占空比 */

            if (speed == 0)
            {
                rt_pin_write(MOTOR_A1, PIN_HIGH);
                rt_pin_write(MOTOR_A2, PIN_HIGH);
            }
            else if(speed > 0)
            {
                rt_pin_write(MOTOR_A1, PIN_HIGH);
                rt_pin_write(MOTOR_A2, PIN_LOW);			
            }
            else
            {
                speed = - speed;
                rt_pin_write(MOTOR_A1, PIN_LOW);
                rt_pin_write(MOTOR_A2, PIN_HIGH);
            }            
        }
			
        speed = speed*10;
        rt_pwm_set(pwm_dev, PWM_MOTOR_CH, 10000, speed);        
        rt_thread_mdelay(10);
    }
}

int motor_thread_init(void)
{
    /* 创建线程 1，名称是 thread1，入口是 thread1_entry*/
    motor_pwm_thread = rt_thread_create("motor",
                            motor_pwm_thread_entry, 
                            RT_NULL,
                            MOTOR_PWM_STACK_SIZE,
                            MOTOR_PWM_PRIORITY, 
                            MOTOR_PWM_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (motor_pwm_thread != RT_NULL)
        rt_thread_startup(motor_pwm_thread);

    return 0;
}

/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(motor_thread_init, motor_thread_init);
INIT_APP_EXPORT(motor_thread_init);





