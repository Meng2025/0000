#ifndef __DBUS_H__
#define __DBUS_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define DBUS_UART       "uart1"      /* 串口设备名称 */

#define DBUS_THREAD_PRIORITY         25
#define DBUS_THREAD_STACK_SIZE       1024
#define DBUS_THREAD_TIMESLICE        10


/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN             ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET          ((uint16_t)1024)
#define RC_CH_VALUE_MAX             ((uint16_t)1684)
/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP                    ((uint16_t)1)
#define RC_SW_MID                   ((uint16_t)3)
#define RC_SW_DOWN                  ((uint16_t)2)
/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W        ((uint16_t)0x01<<0)
#define KEY_PRESSED_OFFSET_S        ((uint16_t)0x01<<1)
#define KEY_PRESSED_OFFSET_A        ((uint16_t)0x01<<2)
#define KEY_PRESSED_OFFSET_D        ((uint16_t)0x01<<3)
#define KEY_PRESSED_OFFSET_Q        ((uint16_t)0x01<<4)
#define KEY_PRESSED_OFFSET_E        ((uint16_t)0x01<<5)
#define KEY_PRESSED_OFFSET_SHIFT    ((uint16_t)0x01<<6)
#define KEY_PRESSED_OFFSET_CTRL     ((uint16_t)0x01<<7)
#define RC_FRAME_LENGTH             18u

/* ----------------------- Data Struct ------------------------------------- */

typedef struct
{
    rt_int16_t  rv;
    rt_int16_t  rh;
    rt_int16_t  lv;
    rt_int16_t  lh;
    rt_int8_t   sl;
    rt_int8_t   sr;
    
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_l;
        uint8_t press_r;
    }mouse;
    
    struct
    {
        uint16_t v;
    }key;
}dbus_t;



extern dbus_t dbus;


#endif



