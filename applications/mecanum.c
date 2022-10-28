#include "mecanum.h"

//#include "sbus.h"
#include "dbus.h"

/*----------------------   pwm 周期占空比初始化配置   --------------------------*/
int mecanum_pwm_init(void)
{
    rt_err_t result = RT_EOK;
    
    struct rt_device_pwm *pwm_dev;
    
    pwm_dev = (struct rt_device_pwm *)rt_device_find(MECANUM_PWM_DEV);
    
    result = rt_pwm_set(pwm_dev, MECANUM_PWM_LF, 10000, 0);
    result = rt_pwm_set(pwm_dev, MECANUM_PWM_LB, 10000, 0);
    result = rt_pwm_set(pwm_dev, MECANUM_PWM_RB, 10000, 0);
    result = rt_pwm_set(pwm_dev, MECANUM_PWM_RF, 10000, 0);    
    
    if (result != RT_EOK)
        rt_kprintf("%s channel config error.\n", MECANUM_PWM_DEV);
    
    result = rt_pwm_enable(pwm_dev, MECANUM_PWM_LF);
    result = rt_pwm_enable(pwm_dev, MECANUM_PWM_LB);
    result = rt_pwm_enable(pwm_dev, MECANUM_PWM_RB);
    result = rt_pwm_enable(pwm_dev, MECANUM_PWM_RF);
    
    if (result != RT_EOK)
        rt_kprintf("%s channel enable error.\n", MECANUM_PWM_DEV);
    return 0;
}

INIT_APP_EXPORT(mecanum_pwm_init);



/*----------------------   底盘控制控制线程   --------------------------*/


static rt_thread_t mecanum_pwm_thread = RT_NULL;

static void mecanum_pwm_thread_entry(void *parameter)
{
    /* x,横向，y纵向，r旋转 控制量 */
    rt_int32_t x, y, r;
    
    /* 麦轮速度解算 */
    rt_int32_t pwm_lf;
    rt_int32_t pwm_lb;
    rt_int32_t pwm_rb;
    rt_int32_t pwm_rf;
    
    struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(MECANUM_PWM_DEV);
    
    rt_pin_mode(MECANUM_LF_A, PIN_MODE_OUTPUT);
    rt_pin_mode(MECANUM_LF_B, PIN_MODE_OUTPUT);
    rt_pin_mode(MECANUM_LB_A, PIN_MODE_OUTPUT);
    rt_pin_mode(MECANUM_LB_B, PIN_MODE_OUTPUT);
    rt_pin_mode(MECANUM_RB_A, PIN_MODE_OUTPUT);
    rt_pin_mode(MECANUM_RB_B, PIN_MODE_OUTPUT);
    rt_pin_mode(MECANUM_RF_A, PIN_MODE_OUTPUT);
    rt_pin_mode(MECANUM_RF_B, PIN_MODE_OUTPUT);
    
    rt_pin_write(MECANUM_LF_A, PIN_HIGH);
    rt_pin_write(MECANUM_LF_B, PIN_HIGH);
    rt_pin_write(MECANUM_LB_A, PIN_HIGH);
    rt_pin_write(MECANUM_LB_B, PIN_HIGH);
    rt_pin_write(MECANUM_RB_A, PIN_HIGH);
    rt_pin_write(MECANUM_RB_B, PIN_HIGH);
    rt_pin_write(MECANUM_RF_A, PIN_HIGH);
    rt_pin_write(MECANUM_RF_B, PIN_HIGH);
    
    while (1)
    {

/* SBUS 遥控器 */        
#ifdef  __SBUS_H__
        x = sbus.ch4;
        y = sbus.ch2;
        r = sbus.ch1;
        
//        mode = sbus.ch5;
        
//        if (mode<1024)
//        {
//            x = x;
//            y = y;
//            r = r;
//        }
//        else
//        {
            /* -300 ~ +300 */
            x = (x - 1024) * 500 / 670;
            y = (y - 1024) * 500 / 670;
            r = (r - 1024) * 500 / 670;
//        }
        

#endif

/* 大疆 遥控器 */
#ifdef  __DBUS_H__
        x = dbus.lh;
        y = dbus.lv;
        r = dbus.rh;
        
        x = (x - 1024) * 500 / 660;
        y = (y - 1024) * 500 / 660;
        r = (r - 1024) * 500 / 660;       
#endif
        
        /* xyr 速度到4轮速度映射 */
        pwm_lf = y + x - r;     /* A */     pwm_rf = y - x + r;     /* B */
        pwm_lb = y - x - r;     /* B */     pwm_rb = y + x + r;     /* A */
        
//        rt_kprintf("A:%4d B:%4d B:%4d A:%4d\n", pwm_lf, pwm_lb, pwm_rf ,pwm_rb);

        /* --------------------------------------------------------------------------------------------------------------- */
        if (pwm_lf>0)                                      
        {
            rt_pin_write(MECANUM_LF_A, PIN_HIGH);                   
            rt_pin_write(MECANUM_LF_B, PIN_LOW);
        }
        else
        {
            pwm_lf = -pwm_lf;
            rt_pin_write(MECANUM_LF_A, PIN_LOW);
            rt_pin_write(MECANUM_LF_B, PIN_HIGH);
        }
                                                                    if (pwm_rf>0)
                                                                    {
                                                                        rt_pin_write(MECANUM_RF_A, PIN_LOW);
                                                                        rt_pin_write(MECANUM_RF_B, PIN_HIGH);
                                                                    }
                                                                    else
                                                                    {
                                                                        pwm_rf = -pwm_rf;
                                                                        rt_pin_write(MECANUM_RF_A, PIN_HIGH);
                                                                        rt_pin_write(MECANUM_RF_B, PIN_LOW);   
                                                                    }
                                                                    
        /* --------------------------------------------------------------------------------------------------------------- */
        if (pwm_lb>0)
        {
            rt_pin_write(MECANUM_LB_A, PIN_HIGH);
            rt_pin_write(MECANUM_LB_B, PIN_LOW);
        }
        else
        {
            pwm_lb = -pwm_lb;
            rt_pin_write(MECANUM_LB_A, PIN_LOW);
            rt_pin_write(MECANUM_LB_B, PIN_HIGH);
        }        
                                                                     if (pwm_rb>0)
                                                                     {
                                                                         rt_pin_write(MECANUM_RB_A, PIN_LOW);
                                                                         rt_pin_write(MECANUM_RB_B, PIN_HIGH);
                                                                     }
                                                                     else
                                                                     {
                                                                         pwm_rb = -pwm_rb;
                                                                         rt_pin_write(MECANUM_RB_A, PIN_HIGH);
                                                                         rt_pin_write(MECANUM_RB_B, PIN_LOW);   
                                                                     }        
        /* --------------------------------------------------------------------------------------------------------------- */
        

        /* pwm信号输出 */
        pwm_lf = pwm_lf * 10;
        pwm_lb = pwm_lb * 10;
        pwm_rb = pwm_rb * 10;
        pwm_rf = pwm_rf * 10;

        rt_pwm_set(pwm_dev, MECANUM_PWM_LF, 10000, pwm_lf);         rt_pwm_set(pwm_dev, MECANUM_PWM_RF, 10000, pwm_rf);
        rt_pwm_set(pwm_dev, MECANUM_PWM_LB, 10000, pwm_lb);         rt_pwm_set(pwm_dev, MECANUM_PWM_RB, 10000, pwm_rb);
        
         

        rt_thread_mdelay(10);
    }
}

int mecanum_thread_init(void)
{
    /* 创建线程 1，名称是 thread1，入口是 thread1_entry*/
    mecanum_pwm_thread = rt_thread_create("mecanum",
                            mecanum_pwm_thread_entry, 
                            RT_NULL,
                            MMECANUM_PWM_STACK_SIZE,
                            MECANUM_PWM_PRIORITY, 
                            MECANUM_PWM_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (mecanum_pwm_thread != RT_NULL)
        rt_thread_startup(mecanum_pwm_thread);

    return 0;
}

/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(motor_thread_init, motor_thread_init);
INIT_APP_EXPORT(mecanum_thread_init);


