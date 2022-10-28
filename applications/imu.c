#include "imu.h"


rt_int32_t yaw = 0;


static rt_thread_t imu_thread = RT_NULL;

static void imu_thread_entry(void *parameter)
{
    while (1)
    {
        mpu_get_data();
        imu_ahrs_update();
        imu_attitude_update(); 
        
        rt_thread_mdelay(5);
        
        yaw = (int)imu.yaw;
        
        rt_kprintf("%d\n",yaw);
        
        rt_thread_mdelay(50);
    }
}



int imu_start(void)
{
  imu_thread = rt_thread_create("imu",
                              imu_thread_entry, 
                              RT_NULL,
                              IMU_THREAD_STACK_SIZE,
                              IMU_THREAD_PRIORITY, 
                              IMU_THREAD_TIMESLICE); 

  if (imu_thread != RT_NULL)
      rt_thread_startup(imu_thread);
  
  return 0;
}

//MSH_CMD_EXPORT(led_flow_start, led flow start);
INIT_APP_EXPORT(imu_start);
















