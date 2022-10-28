#ifndef __SBUS_H__
#define __SBUS_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define DR16_UART       "uart1"      /* 串口设备名称 */

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
    
    rt_uint16_t  ch1;
    rt_uint16_t  ch2;
    rt_uint16_t  ch3;
    rt_uint16_t  ch4;
    rt_uint8_t   swa;
    rt_uint8_t   swb;
    rt_uint8_t   swc;
    rt_uint8_t   swd;
    rt_uint16_t  vra;
    rt_uint16_t  vrb;
}sbus_t;

typedef struct
{
    
    rt_int16_t  speed;
    rt_int16_t  direction;
    rt_int16_t  direction_error;
    rt_int16_t  k_speed;
}sbus_control_t;

#endif



