#include "sbus.h"

#include "servo_pwm.h"

/* 遥控器按键全局数据 */
sbus_t fs_i6x;
sbus_control_t control;
/*--------------------------  控制块  ---------------------------*/

/* 串口设备句柄 */
static rt_device_t serial;

/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};


/* 消息队列控制块 */
static struct rt_messagequeue rx_mq;


/*--------------------------  DMA中断回调  ---------------------------*/

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        /* 消息队列满 */
        rt_kprintf("message queue full！\n");
    }
    return result;
}

/*--------------------------  数据处理线程  ---------------------------*/

/*
    遥控器摇杆收到的数据范围 364-1684  中位 1024

*/

static void dr16_uart1_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;
    static char rx_buffer[RT_SERIAL_RB_BUFSZ + 1];
    
    static char* byte = rx_buffer;

    while (1)
    {
        rt_memset(&msg, 0, sizeof(msg));
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            /* 从串口读取数据*/
            rx_length = rt_device_read(msg.dev, 0, rx_buffer, msg.size);
            
            if (rx_length == 25)
            {
                fs_i6x.ch1 = (byte[1] | byte[2] << 8) & 0x7ff;
                fs_i6x.ch2 = (byte[2] >> 3 | byte[3] << 5) & 0x7ff;
                fs_i6x.ch3 = (byte[3] >> 6 | byte[4] << 2 | byte[5] << 10) & 0x7ff;
                fs_i6x.ch4 = (byte[5] >> 1 | byte[6] << 7) & 0x7ff;
                fs_i6x.swa = (byte[6] >> 4 | byte[7] << 4) & 0x7ff;
                fs_i6x.swb = (byte[7] >> 7 | byte[8] << 1 | byte[9] << 9) & 0x7ff;
                fs_i6x.swc = (byte[9] >> 2 | byte[10] << 6) & 0x7ff;
                fs_i6x.swd = (byte[10] >>5 | byte[11] << 3) & 0x7ff;
                fs_i6x.vra = (byte[12] >>0 | byte[13] << 8) & 0x7ff;
                fs_i6x.vrb = (byte[13] >> 3 | byte[14] << 5) & 0x7ff;
            }
        }
    }
}

int dr16_init(void)
{
    rt_err_t ret = RT_EOK;
    static char msg_pool[256];
    
    /* dr16的初始化配置参数 */
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  
    config.baud_rate = 100000;
    config.data_bits = DATA_BITS_8;
    config.parity    = PARITY_EVEN;
    config.stop_bits = STOP_BITS_2;
    
    /* 查找串口设备 */
    serial = rt_device_find(DR16_UART);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", DR16_UART);
        return RT_ERROR;
    }

    /* 初始化消息队列 */
    rt_mq_init(&rx_mq, "rx_mq",
               msg_pool,                 /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的最大长度 */
               sizeof(msg_pool),         /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);      
    
    /* 以 DMA 接收及轮询发送方式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX);
    
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("dr16", dr16_uart1_thread_entry, RT_NULL, 1024, 25, 10);
    
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }

    return ret;
}

/* 导出命令 or 自动初始化 */
//MSH_CMD_EXPORT(dr16_init, dr16 remote controller init);
INIT_APP_EXPORT(dr16_init);

/*--------------------------  调试输出线程  ---------------------------*/

static void dr16_debug_thread_entry(void *parameter)
{
    while (1)
    {
        rt_kprintf("%4d %4d\n", fs_i6x.ch1, fs_i6x.ch2);
        rt_kprintf("%4d %4d\n", fs_i6x.ch3, fs_i6x.ch4);
        rt_kprintf("%4d %4d\n", fs_i6x.vra, fs_i6x.vrb);
        rt_kprintf("%d %d %d %d\n", fs_i6x.swa, fs_i6x.swb, fs_i6x.swc, fs_i6x.swd);
//        rt_kprintf("%d \n", dr16.mouse.x);
//        rt_kprintf("%d \n", dr16.mouse.y);
//        rt_kprintf("%d \n", dr16.mouse.z);
//        rt_kprintf("%d \n", dr16.mouse.press_l);
//        rt_kprintf("%d \n", dr16.mouse.press_r);
//        rt_kprintf("%d \n", dr16.key.v);
        
        rt_thread_mdelay(1000);
    }
}

static int dr16_output(int argc, char *argv[])
{
    rt_thread_t thread = rt_thread_create("dr16_output", dr16_debug_thread_entry, RT_NULL, 1024, 25, 10);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }

    return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(dr16_output, remote controller data debug outputuart);
