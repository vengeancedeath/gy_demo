//动态任务创建
#include "freertos_semphr.h"

static const char *log_tag = "semaphore";
#define SEMAPHORE_INFO(format, ...) LOG_INFO(log_tag, format, ##__VA_ARGS__)
//#define printf	USB_Printf

#define START_TASK_STACK   (128)
#define START_TASK_PRIORITY (1)
static TaskHandle_t start_task_handle;

static void start_task(void *pvParameters);

QueueHandle_t xQueue1;
QueueHandle_t big_queue;  //字符串

char * big_data = "hello world";
void freertos_start_queue(void)
{
  //创建队列 
  xQueue1 = xQueueCreate(1, sizeof(uint8_t));
//  if(xQueue1 == NULL)
//  {
//    //打印
//    QUEUE_INFO("xQueueCreate failed");
//    return;
//  }
//  else
//  {
//    //打印
//    QUEUE_INFO("xQueueCreate success");
//  }
 
 //创建队列 
 big_queue = xQueueCreate(1, sizeof(char *));
// if(big_queue == NULL)
// {
//   //打印
//   QUEUE_INFO("big_queueCreate failed");
//   return;
// }
// else
// {
//   //打印
//   QUEUE_INFO("big_queueCreate success");
// }
  
  BaseType_t xReturn;
  // Create start_task
  xReturn = xTaskCreate( (TaskFunction_t) start_task,     //任务函数地址
                            (char *) "start_task",  //任务名称
                            (configSTACK_DEPTH_TYPE) START_TASK_STACK,  //任务栈大小
                            (void *) NULL, //任务参数
                            (UBaseType_t) START_TASK_PRIORITY, //任务优先级
                            (TaskHandle_t *) &start_task_handle );  //任务句柄
//  if(xReturn != pdPASS)
//  {
//    //打印
//    QUEUE_INFO("start_task create failed");
//    return;
//  }
//  else
//  {
//    //打印
//    QUEUE_INFO("start_task create success");
//  }
  
  //启动任务调度器 会自带创建空闲任务
  vTaskStartScheduler();
}



#define TASK1_STACK   (128)
#define TASK1_PRIORITY (2)
static TaskHandle_t task1_handle;

#define TASK2_STACK   (84)
#define TASK2_PRIORITY (3)
static TaskHandle_t task2_handle;

#define TASK3_STACK   (84)
#define TASK3_PRIORITY (4)
static TaskHandle_t task3_handle;


static void task1(void *pvParameters);
static void task2(void *pvParameters);
static void task3(void *pvParameters);



static void start_task(void *pvParameters)
{
 
  QUEUE_INFO("start_task: start");
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
                 
// Create task3
  xTaskCreate( (TaskFunction_t) task3,     //任务函数地址
                            (char *) "task3",  //任务名称
                            (configSTACK_DEPTH_TYPE) TASK3_STACK,  //任务栈大小
                            (void *) NULL, //任务参数
                            (UBaseType_t) TASK3_PRIORITY, //任务优先级
                            (TaskHandle_t *) &task3_handle );  //任务句柄



  //退出临界区
 taskEXIT_CRITICAL();
  
  QUEUE_INFO("start_task DELETE");
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
  uint8_t data = 9;
  uint8_t res = 0;

  QUEUE_INFO("task1: running wait 4s");
  vTaskDelay(4000 / portTICK_PERIOD_MS);

  QUEUE_INFO("task1: send data to queue1 %d", data);
  res = xQueueSend(xQueue1, &data, portMAX_DELAY);
  if(res == pdPASS)
  {
    QUEUE_INFO("task1: send data to queue1 success");
  }
  else
  {
    QUEUE_INFO("task1: send data to queue1 failed");
  }

  QUEUE_INFO("task1: running wait 4s after send data");
  vTaskDelay(4000 / portTICK_PERIOD_MS);

  QUEUE_INFO("task1: send data to queue1 %s", big_data);
  res = xQueueSend(big_queue, &big_data, portMAX_DELAY);
  if(res == pdPASS)
  {
    QUEUE_INFO("task1: send data to big_queue success");
  }
  else
  {
    QUEUE_INFO("task1: send data to big_queue failed");
  }
  //任务1循环执行
  while (1)
  {
    // QUEUE_INFO("task1: running");
    
  }
}


static void task2(void *pvParameters)
{
  //任务2循环执行
  QUEUE_INFO("task2: running");
  uint8_t res = 0;
  uint8_t receive = 0;
  
  while (1)
  {
    res = xQueueReceive(xQueue1, &receive, portMAX_DELAY);
    if(res == pdPASS)
    {
      QUEUE_INFO("task2: receive data from queue1 successed %d", receive);
    }
    else
    {
      QUEUE_INFO("task2: receive data from queue1 failed");
    }
  }
}

static void task3(void *pvParameters)
{
  //任务3循环执行
  QUEUE_INFO("task3: running");
  uint8_t res = 0;
  char * receive = NULL;
  while (1)
  {
    res = xQueueReceive(big_queue, &receive, portMAX_DELAY);
    if(res == pdPASS)
    {
      QUEUE_INFO("task3: receive data from big_queue successed = %s", receive);
    }
    else
    {
      QUEUE_INFO("task3: receive data from big_queue failed");
    }
  }
}