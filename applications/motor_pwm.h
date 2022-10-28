#ifndef __MOTOR_PWM_H__
#define __MOTOR_PWM_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


/* H */
#define PWM_MOTOR_DEV		"pwm4"
#define PWM_MOTOR_CH		1

#define MOTOR_A1  GET_PIN(F, 0)
#define MOTOR_A2  GET_PIN(F, 1)


/* pwm电机控制线程 */
#define MOTOR_PWM_PRIORITY         28
#define MOTOR_PWM_STACK_SIZE       1024
#define MOTOR_PWM_TIMESLICE        5




#endif



