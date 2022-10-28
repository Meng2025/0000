#ifndef __MECANUM_H
#define __MECANUM_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define     MECANUM_PWM_DEV         "pwm5"


#define     MECANUM_PWM_LF          4                   /* A */
#define     MECANUM_PWM_LB          3                   /* B */
#define     MECANUM_PWM_RB          2                   /* C */
#define     MECANUM_PWM_RF          1                   /* D */

#define     MECANUM_LF_A            GET_PIN(F, 1)       /* I1 */
#define     MECANUM_LF_B            GET_PIN(F, 0)       /* I2 */
#define     MECANUM_LB_A            GET_PIN(E, 5)       /* J1 */
#define     MECANUM_LB_B            GET_PIN(E, 4)       /* J2 */
#define     MECANUM_RB_A            GET_PIN(E, 6)       /* K1 */
#define     MECANUM_RB_B            GET_PIN(E, 12)      /* K2 */
#define     MECANUM_RF_A            GET_PIN(C, 2)       /* L1 */
#define     MECANUM_RF_B            GET_PIN(B, 0)       /* L2 */



/* pwm电机控制线程 */
#define MECANUM_PWM_PRIORITY        28
#define MMECANUM_PWM_STACK_SIZE     1024
#define MECANUM_PWM_TIMESLICE       5



#endif


