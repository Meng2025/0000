#include "status.h"


#include "ks103.h"



status_machine_t status_machine;

static void status_start(void)
{
    /* 判断状态次数 */
    if (status_machine.current_status != status_machine.last_status)
        status_machine.status_count[status_machine.current_status]++;
    
    /* 状态转移 */
    if (distance > 200 ) 
    {
        status_machine.current_status = RIGHT_RUN;
        status_machine.last_status      = START;
    }

}






static rt_thread_t status = RT_NULL;

static void status_thread(void *parameter)
{
    status_machine.current_status   = START;
    status_machine.last_status      = START;
    status_machine.status_count[status_machine.current_status]++;

    while (1)
    {
        switch (status_machine.current_status)
        {
            case START : ;
            break;
            case OVERTAKE : ;
            break;        
            case CHANGE_LEFT_LANE : ;
            break;
            case CHANGE_RIGHT_LANE : ;
            break;
            case LEFT_RUN: ;
            break;
            case RIGHT_RUN : ;
            break;
             case WAIT : ;
            break;           
             case STOP : ;
            break;
             default : ;
                break;
        }

    }
}

int status_start(void)
{
    status = rt_thread_create("status",
                                      status_thread, 
                                      RT_NULL,
                                      STATUS_STACK_SIZE,
                                      STATUS_PRIORITY, 
                                      STATUS_TIMESLICE);

    if (status != RT_NULL)
        rt_thread_startup(status);
    return 0;
}

//MSH_CMD_EXPORT(led_slow_start, led slow flash start);
INIT_APP_EXPORT(status_start);



