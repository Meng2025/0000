#include "ks103.h"


static const char send_data[3] = {0xe8, 0x02, 0xb0};

rt_uint16_t distance   = 0;



#include <rtthread.h>

#define SAMPLE_UART_NAME       "uart2"      /* 串口设备名称 */

/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};

/* 串口设备句柄 */
static rt_device_t serial;

/* 消息队列控制块 */
static struct rt_messagequeue rx_mq;

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

static void serial_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;
    static char rx_buffer[RT_SERIAL_RB_BUFSZ + 1];

    while (1)
    {
        rt_memset(&msg, 0, sizeof(msg));
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            /* 从串口读取数据*/
            rx_length = rt_device_read(msg.dev, 0, rx_buffer, msg.size);
            
            distance = rx_buffer[0] << 8 | rx_buffer[1];
      
//            rt_kprintf("%d\n",distance);
            
            /* 通过串口设备 serial 输出读取到的消息 */
        }
    }
}

static int uart_dma_sample(void)
{
    rt_err_t ret = RT_EOK;
    static char msg_pool[256];

    /* 查找串口设备 */
    serial = rt_device_find(SAMPLE_UART_NAME);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", SAMPLE_UART_NAME);
        return RT_ERROR;
    }

    /* 初始化消息队列 */
    rt_mq_init(&rx_mq, "rx_mq",
               msg_pool,                 /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的最大长度 */
               sizeof(msg_pool),         /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */        
    /* step2：修改串口配置参数 */
    config.baud_rate = BAUD_RATE_9600;        //修改波特率为 9600
    config.data_bits = DATA_BITS_8;           //数据位 8
    config.stop_bits = STOP_BITS_1;           //停止位 1
    config.bufsz     = 128;                   //修改缓冲区 buff size 为 128
    config.parity    = PARITY_NONE;           //无奇偶校验位
  
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    
    /* 以 DMA 接收及轮询发送方式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 23, 10);
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
/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(uart_dma_sample, uart device dma sample);
INIT_APP_EXPORT(uart_dma_sample);






/* 发送线程 */

/*-------------------------------   线程3：慢闪   -------------------------------*/
static rt_thread_t ks103_send = RT_NULL;

static void ks103_send_thread(void *parameter)
{
    static rt_device_t serial ;
    
    serial = rt_device_find(SAMPLE_UART_NAME);

    while (1)
    {
      rt_device_write(serial, 0, send_data, 3);
      rt_thread_mdelay(50);
    }
}

int ks103_send_start(void)
{
    ks103_send = rt_thread_create("kssend",
                                      ks103_send_thread, 
                                      RT_NULL,
                                      1024,
                                      25, 
                                      10);

    if (ks103_send != RT_NULL)
        rt_thread_startup(ks103_send);
    return 0;
}

INIT_APP_EXPORT(ks103_send_start);







