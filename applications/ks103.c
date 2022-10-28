#include "ks103.h"


static const char send_data[3] = {0xe8, 0x02, 0xbc};

rt_uint16_t distance_lef   = 9999;
rt_uint16_t distance_mid   = 9999;
rt_uint16_t distance_rig   = 9999;

/*---------------------------------------------------------------------------*/

/* 用于接收消息的信号量 */
static struct rt_semaphore  rx_sem_1, rx_sem_2, rx_sem_3;
static rt_device_t          serial_1, serial_2, serial_3;


static rt_err_t ks103_1_uart_cb(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem_1);
    return RT_EOK;
}

static rt_err_t ks103_2_uart_cb(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem_2);
    return RT_EOK;
}

static rt_err_t ks103_3_uart_cb(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem_3);
    return RT_EOK;
}


static void ks103_thread_entry(void *parameter)
{
    char ch;
    char recive_data[2];

    while (1)
    {
        /* 超声波1 */        
        rt_device_write(serial_1, 0, send_data, 3);
        while (rt_device_read(serial_1, -1, &ch, 1) != 1)    /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        {
            rt_sem_take(&rx_sem_1, RT_WAITING_FOREVER);
        }
        recive_data[0] = ch;
        while (rt_device_read(serial_1, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem_1, RT_WAITING_FOREVER);
        }
        recive_data[1] = ch;
        distance_lef = (recive_data[0]<<8) | recive_data[1];
        
         /* 超声波2 */        
        rt_device_write(serial_2, 0, send_data, 3);
        while (rt_device_read(serial_2, -1, &ch, 1) != 1)    /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        {
            rt_sem_take(&rx_sem_2, RT_WAITING_FOREVER);
        }
        recive_data[0] = ch;
        while (rt_device_read(serial_2, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem_2, RT_WAITING_FOREVER);
        }
        recive_data[1] = ch;
        distance_mid = (recive_data[0]<<8) | recive_data[1];       

        /* 超声波3 */        
        rt_device_write(serial_3, 0, send_data, 3);
        while (rt_device_read(serial_3, -1, &ch, 1) != 1)    /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        {
            rt_sem_take(&rx_sem_3, RT_WAITING_FOREVER);
        }
        recive_data[0] = ch;
        while (rt_device_read(serial_3, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rx_sem_3, RT_WAITING_FOREVER);
        }
        recive_data[1] = ch;
        distance_rig = (recive_data[0]<<8) | recive_data[1];

        rt_thread_mdelay(20);
    }
}

static int ks103_thread_init(void)
{
    rt_err_t ret = RT_EOK;
 
    /* 查找系统中的串口设备 */
    serial_1 = rt_device_find(KS103_UART_1);
    if (!serial_1)
    {
        rt_kprintf("find %s failed!\n", KS103_UART_1);
        return RT_ERROR;
    }
    serial_2 = rt_device_find(KS103_UART_2);
    if (!serial_2)
    {
        rt_kprintf("find %s failed!\n", KS103_UART_2);
        return RT_ERROR;
    }
    serial_3 = rt_device_find(KS103_UART_3);
    if (!serial_3)
    {
        rt_kprintf("find %s failed!\n", KS103_UART_3);
        return RT_ERROR;
    }

    
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */
    config.baud_rate = BAUD_RATE_9600;        //修改波特率为 9600
    config.data_bits = DATA_BITS_8;           //数据位 8
    config.stop_bits = STOP_BITS_1;           //停止位 1
    config.parity    = PARITY_NONE;           //无奇偶校验位

    rt_device_control(serial_1, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_control(serial_2, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_control(serial_3, RT_DEVICE_CTRL_CONFIG, &config);    
    
    rt_thread_t thread;
    
    rt_sem_init(&rx_sem_1, "sem_l", 0, RT_IPC_FLAG_FIFO);
    rt_device_open(serial_1, RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial_1, ks103_1_uart_cb);
  
    rt_sem_init(&rx_sem_2, "sem_2", 0, RT_IPC_FLAG_FIFO);
    rt_device_open(serial_2, RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial_2, ks103_2_uart_cb);
  
    rt_sem_init(&rx_sem_3, "sem_3", 0, RT_IPC_FLAG_FIFO);
    rt_device_open(serial_3, RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial_3, ks103_3_uart_cb);   
        
    thread = rt_thread_create("ks103", ks103_thread_entry, RT_NULL, 1024, 25, 10);

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

INIT_APP_EXPORT(ks103_thread_init);

