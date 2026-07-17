//动态定时器创建
#include "freertos_timer.h"

static const char *log_tag = "timer";
#define TIMER_INFO(format, ...) LOG_INFO(log_tag, format, ##__VA_ARGS__)
//#define printf	USB_Printf

#define START_TASK_STACK   (128)
#define START_TASK_PRIORITY (1)
static TaskHandle_t start_task_handle;

static void start_task(void *pvParameters);

void timer1_callback( TimerHandle_t xTimer );
TimerHandle_t timer1_handle;
void timer2_callback( TimerHandle_t xTimer );
TimerHandle_t timer2_handle;

void freertos_start_timer(void)
{

  timer1_handle = xTimerCreate(
    "timer1",  //定时器名称
    (TickType_t) 1000,  //定时器周期 1000 个systick 时钟节拍
    pdTRUE,  //是否重复  pdTRUE 重复执行  pdFALSE 一次执行
    (void *) 1, //定时器编号
    timer1_callback //定时器回调函数地址
  );
  if(timer1_handle == NULL)
  {
    //打印
    TIMER_INFO("timer1 create failed");
    return;
  }

  timer2_handle = xTimerCreate(
    "timer2",  //定时器名称
    (TickType_t) 1000,  //定时器周期 1000 个systick 时钟节拍
    pdFALSE,  //是否重复  pdTRUE 重复执行  pdFALSE 一次执行
    (void *) 2, //定时器编号
    timer2_callback //定时器回调函数地址
  );
  if(timer2_handle == NULL)
  {
    //打印
    TIMER_INFO("timer2 create failed");
    return;
  }




  BaseType_t xReturn;
  // Create start_task
  xReturn = xTaskCreate( (TaskFunction_t) start_task,     //任务函数地址
                            (char *) "start_task",  //任务名称
                            (configSTACK_DEPTH_TYPE) START_TASK_STACK,  //任务栈大小
                            (void *) NULL, //任务参数
                            (UBaseType_t) START_TASK_PRIORITY, //任务优先级
                            (TaskHandle_t *) &start_task_handle );  //任务句柄
  if(xReturn != pdPASS)
  {
    //打印
    TIMER_INFO("start_task create failed");
    return;
  }
  //启动任务调度器 会自带创建空闲任务
  vTaskStartScheduler();
}



#define TASK1_STACK   (128)
#define TASK1_PRIORITY (2)
static TaskHandle_t task1_handle;

#define TASK2_STACK   (128)
#define TASK2_PRIORITY (3)
static TaskHandle_t task2_handle;

#define TASK3_STACK   (128)
#define TASK3_PRIORITY (4)
static TaskHandle_t task3_handle;


static void task1(void *pvParameters);
static void task2(void *pvParameters);
static void task3(void *pvParameters);



static void start_task(void *pvParameters)
{
 
  TIMER_INFO("start_task: start");
  // 进入临界区 作用：保护临界区的代码不会被打断  关闭全部中断
 taskENTER_CRITICAL();
// Create task1
  xTaskCreate( (TaskFunction_t) task1,     //任务函数地址
                            (char *) "task1",  //任务名称
                            (configSTACK_DEPTH_TYPE) TASK1_STACK,  //任务栈大小
                            (void *) NULL, //任务参数
                            (UBaseType_t) TASK1_PRIORITY, //任务优先级
                            (TaskHandle_t *) &task1_handle );  //任务句柄

  
// Create task2
  xTaskCreate( (TaskFunction_t) task2,     //任务函数地址
                            (char *) "task2",  //任务名称
                            (configSTACK_DEPTH_TYPE) TASK2_STACK,  //任务栈大小
                            (void *) NULL, //任务参数
                            (UBaseType_t) TASK2_PRIORITY, //任务优先级
                            (TaskHandle_t *) &task2_handle );  //任务句柄
                 
// // Create task3
//   xTaskCreate( (TaskFunction_t) task3,     //任务函数地址
//                             (char *) "task3",  //任务名称
//                             (configSTACK_DEPTH_TYPE) TASK3_STACK,  //任务栈大小
//                             (void *) NULL, //任务参数
//                             (UBaseType_t) TASK3_PRIORITY, //任务优先级
//                             (TaskHandle_t *) &task3_handle );  //任务句柄



  //退出临界区
 taskEXIT_CRITICAL();
  
  TIMER_INFO("start_task DELETE");
  //删除当前任务
  vTaskDelete(NULL);


  //任务1循环执行
  // while (1)
  // {
  //   TASK_INFO("task0: running");
  //   vTaskDelay(1000 / portTICK_PERIOD_MS);
  // }
}


static void task1(void *pvParameters)
{
  uint8_t res = 0;
  //任务1循环执行
    TIMER_INFO("task1: running");

    vTaskDelay(4000 / portTICK_PERIOD_MS);
    TIMER_INFO("timer1 start");
    res = xTimerStart(timer1_handle, portMAX_DELAY);
    if(res != pdPASS)
    {
      TIMER_INFO("timer1 start failed");
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TIMER_INFO("timer2 start");
    res = xTimerStart(timer2_handle, portMAX_DELAY);
    if(res != pdPASS)
    {
      TIMER_INFO("timer2 start failed");
    }
  while (1)
  {
  }
}


static void task2(void *pvParameters)
{
  uint8_t res = 0;
    TIMER_INFO("task2: running");
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    res = xTimerStop(timer2_handle, portMAX_DELAY);
    if(res != pdPASS)
    {
      TIMER_INFO("timer2 stop failed");
    }
    else
    {
      TIMER_INFO("timer2 stop success");
    }
    res = xTimerStop(timer1_handle, portMAX_DELAY);
    if(res != pdPASS)
    {
      TIMER_INFO("timer1 stop failed");
    }
    else
    {
      TIMER_INFO("timer1 stop success");
    }
  //任务2循环执行
  while (1)
  {
  }
}


void timer1_callback( TimerHandle_t xTimer )
{
  static uint8_t cnt = 0;
  TIMER_INFO("timer1 callback %d", cnt++);
}

void timer2_callback( TimerHandle_t xTimer )
{
  static uint8_t cnt = 0;
  TIMER_INFO("timer2 callback %d", cnt++);
}



static void task3(void *pvParameters)
{
  //任务3循环执行
  while (1)
  {
    TIMER_INFO("task3: running");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}