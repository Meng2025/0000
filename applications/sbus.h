#ifndef __SBUS_H__
#define __SBUS_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define SBUS_UART       "uart1"


/* ----------------------- Data Struct ------------------------------------- */

typedef struct
{
    rt_uint16_t     ch1;
    rt_uint16_t     ch2;
    rt_uint16_t     ch3;
    rt_uint16_t     ch4;
    rt_uint16_t     ch5;
    rt_uint16_t     ch6;
    rt_uint16_t     ch7;
    rt_uint16_t     ch8;
    rt_uint16_t     ch9;
    rt_uint16_t     ch10;
    rt_uint16_t     ch11;
    rt_uint16_t     ch12;
    rt_uint16_t     ch13;
    rt_uint16_t     ch14;
    rt_uint16_t     ch15;
    rt_uint16_t     ch16;
    rt_uint8_t      state;
}sbus_t;


extern sbus_t sbus;


#endif



