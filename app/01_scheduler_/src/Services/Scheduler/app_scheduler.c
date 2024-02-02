/*******************************************************************************/
/**
\file       app_scheduler.c
\brief      Multi-thread Task scheduler.
\author     Abraham Tezmol
\version    0.1
\date       09/09/2008
*/

/** Variable types and common definitions */
#include "system_samv71.h"

/** Scheduler function prototypes definitions */
#include "app_scheduler.h"
/** Tasks definition */
#include "app_tasks.h"
/** Real Time timer resource assigned as scheduler tick */
#include "systick.h"


/*****************************************************************************************************
* Definition of  VARIABLEs - 
*****************************************************************************************************/

UINT8 gu8Scheduler_Status;
UINT8 gu8Scheduler_Counter;

tSchedulerTasks_ID TaskScheduler_Task_ID_Activated;
tSchedulerTasks_ID TaskScheduler_Task_ID_Running;
tSchedulerTasks_ID TasksScheduler_Task_ID_Backup;

UINT8 u8_10ms_Counter;
UINT8 u8_50ms_Counter;
UINT8 u8_100ms_Counter;


tSchedulingTask TimeTriggeredTasks[TASK_SCH_MAX_NUMBER_TIME_TASKS] =
{ 
    {TASKS_1_MS,  TASKS_LIST_1MS,  SUSPENDED, 5},
    {TASKS_2_MS_A,TASKS_LIST_2MS_A,SUSPENDED, 4},
    {TASKS_2_MS_B,TASKS_LIST_2MS_B,SUSPENDED, 4},
    {TASKS_10_MS, TASKS_LIST_10MS, SUSPENDED, 3},
    {TASKS_50_MS, TASKS_LIST_50MS, SUSPENDED, 2},
    {TASKS_100_MS,TASKS_LIST_100MS,SUSPENDED, 1},
};

/*****************************************************************************************************
* Definition of module wide (CONST-) CONSTANTs 
*****************************************************************************************************/


/*****************************************************************************************************
* Code of module wide private FUNCTIONS
*****************************************************************************************************/
void vfnScheduler_Callback(void);

/*****************************************************************************************************
* Code of public FUNCTIONS
*****************************************************************************************************/

/****************************************************************************************************/
/**
* \brief    Scheduler - Initialization
* \author   Abraham Tezmol
* \param    void
* \return   void
* \todo     
*/
void vfnScheduler_Init(void)
{    
    /* Init Global and local Task Scheduler variables */
    gu8Scheduler_Counter   = 0u;
    TaskScheduler_Task_ID_Activated = TASK_NULL;
    TaskScheduler_Task_ID_Running = TASK_NULL;
    TasksScheduler_Task_ID_Backup = TASK_NULL;
    u8_10ms_Counter        = 0u;
    u8_50ms_Counter        = 0u;
    u8_100ms_Counter       = 0u;
    gu8Scheduler_Status    = TASK_SCHEDULER_INIT;
}

/*******************************************************************************/
/**
* \brief    Scheduler Start - Once time base is armed, start execution of   \n
            Multi-thread Round Robin scheduling scheme.                     \n
            This function requires prior execution of "vfnScheduler_Init"
* \author   Abraham Tezmol
* \param    void
* \return   void
* \todo     
*/
void vfnScheduler_Start(void)
{
	if (sysTick_init(TASK_SCHEDULER_BASE_FREQ, vfnScheduler_Callback)) 
	{
		while (1);
	}
    gu8Scheduler_Status = TASK_SCHEDULER_RUNNING;
}

/*******************************************************************************/
/**
* \brief    Scheduler Stop - stop execution of Multi-thread Round Robin scheduling scheme.
* \author   Abraham Tezmol
* \param    void
* \return   void
* \todo     
*/
void vfnScheduler_Stop(void)
{  
    /* Update scheduler status accordingly */
    gu8Scheduler_Status = TASK_SCHEDULER_HALTED;
}

/***************************************************************************************************/
/**
* \brief    Scheduler - execution of time or event driven tasks
* \author   Abraham Tezmol
* \param    void
* \return   void
* \todo     
*/
void vfnScheduler_TaskStart( tSchedulingTask * Task )
{
    /* Indicate that this Task has gained CPU allocation */ 
    Task->enTaskState = RUNNING;
    TaskScheduler_Task_ID_Running =  Task->TaskId;
    /* Perform actual execution of task */
    Task->ptrTask();
    /* Indicate that Task execution has completed */ 
    Task->enTaskState = SUSPENDED;
}

/***************************************************************************************************/
/**
* \brief    Scheduler - activation of time or event driven tasks
* \author   Abraham Tezmol
* \param    Task - pointer to task
* \return   void
* \todo     
*/
void vfnScheduler_TaskActivate( tSchedulingTask * Task )
{ 
    TaskScheduler_Task_ID_Activated = Task->TaskId;
    Task->enTaskState = READY;
}

/*******************************************************************************/
/**
* \brief    Multi-thread round robin task Scheduler  (non-preemtive)        \n
            It calls the different tasks based on the status of             \n   
            "gu8Scheduler_Thread_ID". This variable is modified by          \n
            ISR "vfnScheduler_PIT_Isr"                                        \n
            List of tasks shall be defined @ "tasks.h" file
* \author   Abraham Tezmol
* \param    void
* \return   void
* \todo     
*/
void vfnTask_Scheduler(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*  1ms execution thread - used to derive two execution threads:                */
    /*  a) 1ms thread (high priority tasks)                                         */
    /*  b) 100ms thread (lowest priority tasks)                                     */
    /*  As any other thread on this scheduler, all tasks must be executed in <=500us*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if( ( TaskScheduler_Task_ID_Activated == TASKS_1_MS )
         || ( TaskScheduler_Task_ID_Activated == TASKS_100_MS ) )
    {
        /* Make a copy of scheduled task ID */
        TasksScheduler_Task_ID_Backup = TaskScheduler_Task_ID_Activated;

        vfnScheduler_TaskStart (&TimeTriggeredTasks[TASKS_1_MS]);
        if( TaskScheduler_Task_ID_Activated == TASKS_100_MS )
        {
            vfnScheduler_TaskStart (&TimeTriggeredTasks[TASKS_100_MS]);
        }
        /* Verify that thread execution took less than 500 us */
        if( TasksScheduler_Task_ID_Backup == TaskScheduler_Task_ID_Activated )
        {
             /* In case execution of all thread took less than 500us */
            TaskScheduler_Task_ID_Activated = TASK_NULL;
        }
        else
        {
            gu8Scheduler_Status = TASK_SCHEDULER_OVERLOAD_1MS;
        }
    }
    else
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        /*  2ms execution thread - used to derive two execution threads:                */
        /*  a) 2ms group A thread (high priority tasks)                                 */
        /*  b) 50ms thread (second lowest priority tasks)                               */
        /*  As any other thread on this scheduler, all tasks must be executed in <=500us*/
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        if( ( TaskScheduler_Task_ID_Activated == TASKS_2_MS_A )
             || ( TaskScheduler_Task_ID_Activated == TASKS_50_MS ) )
        {
            /* Make a copy of scheduled task ID */
            TasksScheduler_Task_ID_Backup = TaskScheduler_Task_ID_Activated;

            vfnScheduler_TaskStart (&TimeTriggeredTasks[TASKS_2_MS_A]);
            if( TaskScheduler_Task_ID_Activated == TASKS_50_MS )
            {
                vfnScheduler_TaskStart (&TimeTriggeredTasks[TASKS_50_MS]);
            }
            /* Verify that thread execution took less than 500 us */
            if( TasksScheduler_Task_ID_Backup == TaskScheduler_Task_ID_Activated )
            {
                 /* In case execution of all thread took less than 500us */
                TaskScheduler_Task_ID_Activated = TASK_NULL;
            }
            else
            {
                gu8Scheduler_Status = TASK_SCHEDULER_OVERLOAD_2MS_A;
            }
        }
        else
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            /*  2ms execution thread - used to derive two execution threads:                */
            /*  a) 2ms group B thread (high priority tasks)                                 */
            /*  b) 10ms thread (medium priority tasks)                                      */
            /*  As any other thread on this scheduler, all tasks must be executed in <=500us*/
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            if( ( TaskScheduler_Task_ID_Activated == TASKS_2_MS_B )
                 || ( TaskScheduler_Task_ID_Activated == TASKS_10_MS ) )
            {
                /* Make a copy of scheduled task ID */
                TasksScheduler_Task_ID_Backup = TaskScheduler_Task_ID_Activated;

                vfnScheduler_TaskStart (&TimeTriggeredTasks[TASKS_2_MS_B]);
                if( TaskScheduler_Task_ID_Activated == TASKS_10_MS )
                {
                    vfnScheduler_TaskStart (&TimeTriggeredTasks[TASKS_10_MS]);
                }
                 /* Verify that thread execution took less than 500 us */
                if( TasksScheduler_Task_ID_Backup == TaskScheduler_Task_ID_Activated )
                {
                    /* In case execution of all thread took less than 500us */
                    TaskScheduler_Task_ID_Activated = TASK_NULL;
                }
                else
                {
                    gu8Scheduler_Status = TASK_SCHEDULER_OVERLOAD_2MS_B;
                }
            }
        }
    }        
}

/*******************************************************************************/
/**
* \brief    Periodic Interrupt Timer Service routine.                            \n
            This interrupt is the core of the task scheduler.                   \n
            It is executed every 500us                                          \n
            It defines 3 basic threads from which other 3 threads are derived:  \n
            a) 1ms thread (basic) ->  100ms thread (derived)                    \n
            b) 2ms A thread (basic)-> 50ms thread (derived)                     \n
            c) 2ms B thread (basic)-> 10ms thread (derived)                     \n
            It partitions core execution time into time slices (500us each one).\n 
            This arrangement assures core will have equal task loading across time.\n   
            For more information on how time slice is assigned to each thread,  \n
            refer to file "S12X Task Scheduler Layout.xls"
* \author   Abraham Tezmol
* \param    void
* \return   void
* \todo
*/

void vfnScheduler_Callback(void)
{
    /*-- Update scheduler control variable --*/
    gu8Scheduler_Counter++;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*  1ms execution thread - used to derive two execution threads:                */
    /*  a) 1ms thread (highest priority tasks)                                      */
    /*  b) 100ms thread (lowest priority tasks)                                     */
    /*  As any other thread on this scheduling scheme,                              */
    /*  all tasks must be executed in <= 500us                                      */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if( ( gu8Scheduler_Counter & 0x01u ) == 0x01u )
    {
        u8_100ms_Counter++;
        /*-- Allow 100 ms periodic tasks to be executed --*/
        if( u8_100ms_Counter >= 100u )
        {
            /* Indicate that Task is Ready to be executed */ 
            vfnScheduler_TaskActivate(&TimeTriggeredTasks[TASKS_100_MS]);
            u8_100ms_Counter       = 0u;
        }
        /*-- Allow 1 ms periodic tasks to be executed --*/
        else
        {
            vfnScheduler_TaskActivate(&TimeTriggeredTasks[TASKS_1_MS]);
        }
    }
    else
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        /*  2ms execution thread - used to derive two execution threads:                */
        /*  a) 2ms group A thread (high priority tasks)                                 */
        /*  b) 50ms thread (second lowest priority tasks)                               */
        /*  As any other thread on this scheduling scheme,                              */
        /*  all tasks must be executed in <= 500us                                      */
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        if( ( gu8Scheduler_Counter & 0x02u ) == 0x02u )
        {
            u8_50ms_Counter++;
            /*-- Allow 50 ms periodic tasks to be executed --*/
            if( u8_50ms_Counter >= 25u )
            {
                vfnScheduler_TaskActivate(&TimeTriggeredTasks[TASKS_50_MS]);
                u8_50ms_Counter        = 0u;
            }
            /*-- Allow 2 ms group A periodic tasks to be executed --*/
            else
            {
                vfnScheduler_TaskActivate(&TimeTriggeredTasks[TASKS_2_MS_A]);
            }
        }
        else
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            /*  2ms execution thread - used to derive two execution threads:                */
            /*  a) 2ms group B thread (high priority tasks)                                 */
            /*  b) 10ms thread (medium priority tasks)                                      */
            /*  As any other thread on this scheduling scheme,                              */
            /*  all tasks must be executed in <= 500us                                      */
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            if( ( gu8Scheduler_Counter & 0x03u ) == 0x00u )
            {
                u8_10ms_Counter++;
                /*-- Allow 10 ms periodic tasks to be executed --*/
                if( u8_10ms_Counter >= 5u )
                {
                    vfnScheduler_TaskActivate(&TimeTriggeredTasks[TASKS_10_MS]);
                    u8_10ms_Counter        = 0u;
                }
                /*-- Allow 2 ms group B periodic tasks to be executed --*/
                else
                {
                    vfnScheduler_TaskActivate(&TimeTriggeredTasks[TASKS_2_MS_B]);
                }
            }
        }
    }
}
/***************************************************************************************************/
