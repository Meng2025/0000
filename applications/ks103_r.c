#include "ks103_r.h"


static const char send_data[3] = {0xe8, 0x02, 0xbc};

rt_uint16_t distance_right   = 9999;


/*---------------------------------------------------------------------------*/

/* 用于接收消息的信号量 */
static struct rt_semaphore  rx_sem;
static rt_device_t          serial;


static rt_err_t ks103_r_uart_cb(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem);
    return RT_EOK;
}

static void ks103_r_uart_thread_entry(void *parameter)
{
    char ch;
    char recive_data[2];

    while (1)
    {
        rt_device_write(serial, 0, send_data, 3);
        
        /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
        
        recive_data[0] = ch;
        
        /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
        
        recive_data[1] = ch;
        
        distance_right = (recive_data[0]<<8) | recive_data[1];
   
        rt_thread_mdelay(20);
    }
}

static int ks103_r_uart_thread_init(void)
{
    rt_err_t ret = RT_EOK;
 
    /* 查找系统中的串口设备 */
    serial = rt_device_find(KS103_UART_R);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", KS103_UART_R);
        return RT_ERROR;
    }
    
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */
    
    config.baud_rate = BAUD_RATE_9600;        //修改波特率为 9600
    config.data_bits = DATA_BITS_8;           //数据位 8
    config.stop_bits = STOP_BITS_1;           //停止位 1
    config.parity    = PARITY_NONE;           //无奇偶校验位

    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    
    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "sem_r", 0, RT_IPC_FLAG_FIFO);
    /* 以中断接收及轮询发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, ks103_r_uart_cb);

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("ks103_r", ks103_r_uart_thread_entry, RT_NULL, 1024, 25, 10);
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

INIT_APP_EXPORT(ks103_r_uart_thread_init);

