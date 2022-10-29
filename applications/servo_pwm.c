#include <stdlib.h>
#include "servo_pwm.h"
#include "dbus.h"

/*----------------------   pwm周期占空比初始化配置   --------------------------*/
int servo_pwm_init(void)
{
    rt_err_t result = RT_EOK;

    struct rt_device_pwm *pwm_dev;
    
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_SERVO_DEV);
    
    result = rt_pwm_set(pwm_dev, PWM_SERVO_CH, 20000000, 1500000);
    
    if (result != RT_EOK)
        rt_kprintf("%s channel %d config error.\n", PWM_SERVO_DEV, PWM_SERVO_CH);
    
    result = rt_pwm_enable(pwm_dev, PWM_SERVO_CH);
    
    if (result != RT_EOK)
        rt_kprintf("%s channel enable error.\n", PWM_SERVO_DEV);
    return 0;
}

INIT_APP_EXPORT(servo_pwm_init);


/*----------------------   pwm舵机控制线程   --------------------------*/


static rt_thread_t servo_pwm_thread = RT_NULL;


/* 线程 1 的入口函数 */
static void servo_pwm_thread_entry(void *parameter)
{
    rt_int32_t pwm_duty;
	
    struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */
    
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_SERVO_DEV);
    
    while (1)
    {
        pwm_duty = dbus.rh;

        pwm_duty = (1024 - pwm_duty) * 150 / 660; 

        pwm_duty = (1500 - pwm_duty + 20)*1000;
    
        rt_pwm_set(pwm_dev, PWM_SERVO_CH, 20000000, pwm_duty);   			

        rt_thread_mdelay(10);
    }
}

int servo_thread_init(void)
{
    /* 创建线程 1，名称是 thread1，入口是 thread1_entry*/
    servo_pwm_thread = rt_thread_create("servo",
                            servo_pwm_thread_entry, 
                            RT_NULL,
                            SERVO_PWM_STACK_SIZE,
                            SERVO_PWM_PRIORITY, 
                            SERVO_PWM_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (servo_pwm_thread != RT_NULL)
        rt_thread_startup(servo_pwm_thread);

    return 0;
}

/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(motor_thread_init, motor_thread_init);
INIT_APP_EXPORT(servo_thread_init);


