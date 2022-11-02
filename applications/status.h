#ifndef __STATUS_H__
#define __STATUS_H__


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define STATUS_PRIORITY         20
#define STATUS_STACK_SIZE       256
#define STATUS_TIMESLICE        5



typedef enum {
    START    ,      /* 出发区等待 */
    CHANGE_LEFT_LANE ,
    CHANGE_RIGHT_LANE ,
    LEFT_RUN,
    RIGHT_RUN,
    STOP,
    STATUS_NUM,
} status_t;

typedef struct {
    status_t    current_status;
    status_t    last_status;
    rt_int16_t  status_count[STATUS_NUM];
} status_machine_t;



#endif


