//静态任务创建
//开启静态任务宏
#include "freertos_task_static.h"

static const char *log_tag = "task_static";
#define TASK_STATIC_INFO(format, ...) LOG_INFO(log_tag, format, ##__VA_ARGS__)
//#define printf	USB_Printf

// #define START_TASK_STACK   (128)
// #define START_TASK_PRIORITY (1)
// TaskHandle_t start_task_handle;

#define START_TASK_STATIC_STACK   (128)
#define START_TASK_STATIC_PRIORITY (1)
TaskHandle_t start_task_static_handle;
StackType_t static_stack_stack_buffer[START_TASK_STATIC_STACK];
StaticTask_t static_tcb_buffer;

// static void start_task(void *pvParameters);
static void start_task_static(void *pvParameters);


/*静态任务创建需要手动创建两个任务的资源 ：空闲任务和软件定时器*/
//分配空闲任务的资源
StackType_t idle_task_stack_buffer[configMINIMAL_STACK_SIZE];
StaticTask_t idle_task_tcb_buffer;
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE * puxIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &idle_task_tcb_buffer;
  *ppxIdleTaskStackBuffer = idle_task_stack_buffer;
  *puxIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


StackType_t timer_task_stack_buffer[configTIMER_TASK_STACK_DEPTH];
StaticTask_t timer_task_tcb_buffer;
//分配软件定时器的资源
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                      StackType_t ** ppxTimerTaskStackBuffer,
                                      configSTACK_DEPTH_TYPE * puxTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &timer_task_tcb_buffer;
  *ppxTimerTaskStackBuffer = timer_task_stack_buffer;
  *puxTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void freertos_start_static(void)
{
  // Create start_task
  // xTaskCreate( (TaskFunction_t) start_task,     //任务函数地址
  //                           (char *) "start_task",  //任务名称
  //                           (configSTACK_DEPTH_TYPE) START_TASK_STACK,  //任务栈大小
  //                           (void *) NULL, //任务参数
  //                           (UBaseType_t) START_TASK_PRIORITY, //任务优先级
  //                           (TaskHandle_t *) &start_task_handle );  //任务句柄

  start_task_static_handle = xTaskCreateStatic( (TaskFunction_t) start_task_static,     //任务函数地址
                                (char *) "start_task_static",  //任务名称
                                (uint32_t) START_TASK_STATIC_STACK,  //任务栈大小
                                (void *) NULL, //任务参数
                                (UBaseType_t) START_TASK_STATIC_PRIORITY, //任务优先级

                                (StackType_t *) static_stack_stack_buffer,  //任务堆栈
                                (StaticTask_t *) &static_tcb_buffer );  //任务控制块指针
  //启动任务调度器 会自带创建空闲任务和软件定时器（若开启软件定时器功能）
  vTaskStartScheduler();
}



// #define TASK1_STACK   (128)
// #define TASK1_PRIORITY (2)
// TaskHandle_t task1_handle;

// #define TASK2_STACK   (128)
// #define TASK2_PRIORITY (3)
// TaskHandle_t task2_handle;

// #define TASK3_STACK   (128)
// #define TASK3_PRIORITY (4)
// TaskHandle_t task3_handle;


// static void task1(void *pvParameters);
// static void task2(void *pvParameters);
// static void task3(void *pvParameters);

#define TASK1_STATIC_STACK   (128)
#define TASK1_STATIC_PRIORITY (2)
TaskHandle_t task1_static_handle;
StackType_t static_stack1_stack_buffer[TASK1_STATIC_STACK];
StaticTask_t static_tcb1_buffer;

#define TASK2_STATIC_STACK   (128)
#define TASK2_STATIC_PRIORITY (3)
TaskHandle_t task2_static_handle;
StackType_t static_stack2_stack_buffer[TASK2_STATIC_STACK];
StaticTask_t static_tcb2_buffer;

#define TASK3_STATIC_STACK   (128)
#define TASK3_STATIC_PRIORITY (4)
TaskHandle_t task3_static_handle;
StackType_t static_stack3_stack_buffer[TASK3_STATIC_STACK];
StaticTask_t static_tcb3_buffer;


static void task1_static(void *pvParameters);
static void task2_static(void *pvParameters);
static void task3_static(void *pvParameters);


static void start_task_static(void *pvParameters)
{
 
  TASK_STATIC_INFO("start_task_static: start");
  // 进入临界区 作用：保护临界区的代码不会被打断  关闭全部中断
 taskENTER_CRITICAL();
// // Create task1
//   xTaskCreate( (TaskFunction_t) task1,     //任务函数地址
//                             (char *) "task1",  //任务名称
//                             (configSTACK_DEPTH_TYPE) TASK1_STACK,  //任务栈大小
//                             (void *) NULL, //任务参数
//                             (UBaseType_t) TASK1_PRIORITY, //任务优先级
//                             (TaskHandle_t *) &task1_handle );  //任务句柄

  
// // Create task2
//   xTaskCreate( (TaskFunction_t) task2,     //任务函数地址
//                             (char *) "task2",  //任务名称
//                             (configSTACK_DEPTH_TYPE) TASK2_STACK,  //任务栈大小
//                             (void *) NULL, //任务参数
//                             (UBaseType_t) TASK2_PRIORITY, //任务优先级
//                             (TaskHandle_t *) &task2_handle );  //任务句柄
                 
// // Create task3
//   xTaskCreate( (TaskFunction_t) task3,     //任务函数地址
//                             (char *) "task3",  //任务名称
//                             (configSTACK_DEPTH_TYPE) TASK3_STACK,  //任务栈大小
//                             (void *) NULL, //任务参数
//                             (UBaseType_t) TASK3_PRIORITY, //任务优先级
//                             (TaskHandle_t *) &task3_handle );  //任务句柄


                            
  task1_static_handle = xTaskCreateStatic( (TaskFunction_t) task1_static,     //任务函数地址
                                (char *) "task1_static",  //任务名称
                                (uint32_t) TASK1_STATIC_STACK,  //任务栈大小
                                (void *) NULL, //任务参数
                                (UBaseType_t) TASK1_STATIC_PRIORITY, //任务优先级

                                (StackType_t *) static_stack1_stack_buffer,  //任务堆栈
                                (StaticTask_t *) &static_tcb1_buffer );  //任务控制块指针

                            
  task2_static_handle = xTaskCreateStatic( (TaskFunction_t) task2_static,     //任务函数地址
                                (char *) "task2_static",  //任务名称
                                (uint32_t) TASK2_STATIC_STACK,  //任务栈大小
                                (void *) NULL, //任务参数
                                (UBaseType_t) TASK2_STATIC_PRIORITY, //任务优先级

                                (StackType_t *) static_stack2_stack_buffer,  //任务堆栈
                                (StaticTask_t *) &static_tcb2_buffer );  //任务控制块指针

  
                            
  task3_static_handle = xTaskCreateStatic( (TaskFunction_t) task3_static,     //任务函数地址
                                (char *) "task3_static",  //任务名称
                                (uint32_t) TASK3_STATIC_STACK,  //任务栈大小
                                (void *) NULL, //任务参数
                                (UBaseType_t) TASK3_STATIC_PRIORITY, //任务优先级

                                (StackType_t *) static_stack3_stack_buffer,  //任务堆栈
                                (StaticTask_t *) &static_tcb3_buffer );  //任务控制块指针
  //退出临界区
 taskEXIT_CRITICAL();
  
  TASK_STATIC_INFO("start_task_static DELETE");
  //删除当前任务
  vTaskDelete(NULL);


  //任务1循环执行
  // while (1)
  // {
  //   TASK_INFO("task0: running");
  //   vTaskDelay(1000 / portTICK_PERIOD_MS);
  // }
}


static void task1_static(void *pvParameters)
{
  //任务1循环执行
  while (1)
  {
    TASK_STATIC_INFO("task1_static: running");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


static void task2_static(void *pvParameters)
{
  //任务2循环执行
  while (1)
  {
    TASK_STATIC_INFO("task2_static: running");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

static void task3_static(void *pvParameters)
{
  //任务3循环执行
  while (1)
  {
    TASK_STATIC_INFO("task3_static: running");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}