#ifndef __CAR_LIGHT_H__
#define __CAR_LIGHT_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define CAR_LIGHT_PRIORITY         28
#define CAR_LIGHT_STACK_SIZE       256
#define CAR_LIGHT_TIMESLICE        5




#define LIGHT_FRONT_1       GET_PIN(E, 4)
#define LIGHT_FRONT_2       GET_PIN(E, 5)

#define LIGHT_BREAK_1       GET_PIN(E, 6)
#define LIGHT_BREAK_2       GET_PIN(E, 12)

#define LIGHT_LEFT_1        GET_PIN(C, 2)
#define LIGHT_LEFT_2        GET_PIN(B, 0)

#define LIGHT_RIGHT_1       GET_PIN(C, 3)
#define LIGHT_RIGHT_2       GET_PIN(B, 1)

#define LIGHT_TEST_KEY      GET_PIN(B, 2)

#endif



