#ifndef __JETSON_NANO_H__
#define __JETSON_NANO_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define JETSON_UART       "uart3"      /* 串口设备名称 */



#define NANO_PRIORITY         24
#define NANO_STACK_SIZE       1024
#define NANO_TIMESLICE        5

extern rt_int32_t nano_data;


#endif

