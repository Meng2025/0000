#ifndef __SERVO_PWM_H__
#define __SERVO_PWM_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>



/* A */
#define PWM_SERVO_DEV       "pwm5"
#define PWM_SERVO_CH        4

/* pwm电机控制线程 */
#define SERVO_PWM_PRIORITY         28
#define SERVO_PWM_STACK_SIZE       1024
#define SERVO_PWM_TIMESLICE        5



#endif



