#include "mecanum.h"

//#include "sbus.h"
#include "dbus.h"

#include "imu.h"
#include "ks103.h"

struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */

/*----------------------   pwm 周期占空比初始化配置   --------------------------*/
int mecanum_pwm_init(void)
{
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
    
    rt_err_t result = RT_EOK;
    
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


/* 解算方法 */

static void motor_pwm_set1(rt_int32_t _p)
{
    rt_int32_t __p = 0;
    __p = _p;
    
    if (_p >= 0)
    {
        rt_pin_write(MECANUM_LF_A, PIN_LOW);
        rt_pin_write(MECANUM_LF_B, PIN_HIGH);
    }
    else
    {
        rt_pin_write(MECANUM_LF_A, PIN_HIGH);
        rt_pin_write(MECANUM_LF_B, PIN_LOW);
        __p = -__p;
    }
 
    __p = 10*__p;
    
    rt_pwm_set(pwm_dev, MECANUM_PWM_LF, 10000, __p);
}


static void motor_pwm_set2(rt_int32_t _p)
{
    rt_int32_t __p = 0;
    __p = _p;
    
    if (_p >= 0)
    {
        rt_pin_write(MECANUM_LB_A, PIN_LOW);
        rt_pin_write(MECANUM_LB_B, PIN_HIGH);
    }
    else
    {
        rt_pin_write(MECANUM_LB_A, PIN_HIGH);
        rt_pin_write(MECANUM_LB_B, PIN_LOW);
        __p = -__p;
    }
 
    __p = 10*__p;
    
    rt_pwm_set(pwm_dev, MECANUM_PWM_LB, 10000, __p);
}

static void motor_pwm_set3(rt_int32_t _p)
{
    rt_int32_t __p = 0;
    __p = _p;
    
    if (_p >= 0)
    {
        rt_pin_write(MECANUM_RB_A, PIN_HIGH);
        rt_pin_write(MECANUM_RB_B, PIN_LOW);
    }
    else
    {
        rt_pin_write(MECANUM_RB_A, PIN_LOW);
        rt_pin_write(MECANUM_RB_B, PIN_HIGH);
        __p = -__p;
    }
 
    __p = 10*__p;
    
    rt_pwm_set(pwm_dev, MECANUM_PWM_RB, 10000, __p);
}

static void motor_pwm_set4(rt_int32_t _p)
{
    rt_int32_t __p = 0;
    __p = _p;
    
    if (_p >= 0)
    {
        rt_pin_write(MECANUM_RF_A, PIN_HIGH);
        rt_pin_write(MECANUM_RF_B, PIN_LOW);
    }
    else
    {
        rt_pin_write(MECANUM_RF_A, PIN_LOW);
        rt_pin_write(MECANUM_RF_B, PIN_HIGH);
        __p = -__p;
    }
 
    __p = 10*__p;
    
    rt_pwm_set(pwm_dev, MECANUM_PWM_RF, 10000, __p);
}


static void chassis_control(rt_int32_t _x, rt_int32_t _y, rt_int32_t _r)
{
    rt_int32_t _lf = 0;
    rt_int32_t _lb = 0;    
    rt_int32_t _rb = 0;
    rt_int32_t _rf = 0;
    
    
    _lf = _y + _x + _r;
    _lb = _y - _x + _r;
    _rb = _y + _x - _r;
    _rf = _y - _x - _r;
    
//    rt_kprin("%d %d %d %d\n",_lf, _lb, _rb, _rf);
    
    
//    _lf = _x - _y - _r;
//    _lb = _x + _y - _r;
//    _rb = _x + _y + _r;
//    _rf = _x - _y + _r;
        
	motor_pwm_set1(_lf);
	motor_pwm_set2(_lb);
	motor_pwm_set3(_rb);
	motor_pwm_set4(_rf);
}


/*----------------------   底盘控制控制线程   --------------------------*/


static rt_thread_t mecanum_pwm_thread = RT_NULL;

static void mecanum_pwm_thread_entry(void *parameter)
{
    rt_int32_t x = 0;
    rt_int32_t y = 0;
    rt_int32_t r = 0;

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

        /* 横向比例控制 */
//        x = 2*(200 - distance_left);
        
        /* 旋转比例控制 */
//        r = 2*(r_target - imu.yaw);
        
   
        chassis_control(x, y, r);

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


