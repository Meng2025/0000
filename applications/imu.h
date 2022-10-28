#ifndef __IMU_H
#define __IMU_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "bsp_imu.h"

#define IMU_THREAD_PRIORITY         25
#define IMU_THREAD_STACK_SIZE       512
#define IMU_THREAD_TIMESLICE        5

extern rt_int32_t yaw;

#endif

