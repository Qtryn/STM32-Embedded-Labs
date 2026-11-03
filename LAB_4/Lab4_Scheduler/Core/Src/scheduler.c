#include "scheduler.h"
#include <stddef.h>

/* Information of one task */
typedef struct
{
    void (*pTask)(void);
    uint32_t NextRun;
    uint32_t Period;
    uint32_t TaskID;
} sTask;

/* Task list */
static sTask tasks[SCH_MAX_TASKS];

/* Global time counter (10 ms per tick) */
static volatile uint32_t ticks = 0U;

/* Generate unique task IDs */
static uint32_t nextID = 1U;


/* =========================================
   1. INITIALIZE SCHEDULER
   ========================================= */
void SCH_Init(void)
{
    ticks = 0U;
    nextID = 1U;

    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++)
    {
        tasks[i].pTask = NULL;
        tasks[i].NextRun = 0U;
        tasks[i].Period = 0U;
        tasks[i].TaskID = SCH_INVALID_ID;
    }
}


/* =========================================
   2. UPDATE TIME - CALLED BY TIM2 INTERRUPT
   ========================================= */
void SCH_Update(void)
{
    ticks++;
}


/* =========================================
   3. ADD TASK
   DELAY and PERIOD are in 10 ms ticks
   ========================================= */
uint32_t SCH_Add_Task(
    void (*pFunction)(void),
    uint32_t DELAY,
    uint32_t PERIOD
)
{
    if (pFunction == NULL)
    {
        return SCH_INVALID_ID;
    }

    /* Find an empty slot */
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++)
    {
        if (tasks[i].pTask == NULL)
        {
            uint32_t id = nextID++;

            if (id == SCH_INVALID_ID)
            {
                id = nextID++;
            }

            tasks[i].pTask = pFunction;
            tasks[i].NextRun = ticks + DELAY;
            tasks[i].Period = PERIOD;
            tasks[i].TaskID = id;

            return id;
        }
    }

    /* Task list is full */
    return SCH_INVALID_ID;
}


/* =========================================
   4. DELETE TASK BY ID
   ========================================= */
uint8_t SCH_Delete_Task(uint32_t taskID)
{
    if (taskID == SCH_INVALID_ID)
    {
        return 0U;
    }

    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++)
    {
        if (tasks[i].pTask != NULL &&
            tasks[i].TaskID == taskID)
        {
            tasks[i].pTask = NULL;
            tasks[i].NextRun = 0U;
            tasks[i].Period = 0U;
            tasks[i].TaskID = SCH_INVALID_ID;

            return 1U;
        }
    }

    return 0U;
}


/* =========================================
   5. GET CURRENT TIME IN MILLISECONDS
   ========================================= */
uint32_t get_time(void)
{
    return ticks * SCH_TICK_MS;
}


/* =========================================
   6. DISPATCH READY TASKS
   Called in main while(1), NOT in ISR
   ========================================= */
void SCH_Dispatch_Tasks(void)
{
    while (1)
    {
        int32_t selected = -1;
        uint32_t now = ticks;

        /* Find the earliest task that is due */
        for (uint32_t i = 0; i < SCH_MAX_TASKS; i++)
        {
            if (tasks[i].pTask != NULL &&
                (int32_t)(now - tasks[i].NextRun) >= 0)
            {
                if (selected == -1 ||
                    (int32_t)(
                        tasks[i].NextRun -
                        tasks[selected].NextRun
                    ) < 0)
                {
                    selected = (int32_t)i;
                }
            }
        }

        /* No task is ready */
        if (selected == -1)
        {
            return;
        }

        sTask *task = &tasks[selected];

        void (*function)(void) = task->pTask;

        if (task->Period == 0U)
        {
            /* One-shot: run only once */
            SCH_Delete_Task(task->TaskID);
        }
        else
        {
            /* Periodic: calculate next deadline */
            task->NextRun += task->Period;
        }

        /* Execute the task outside the interrupt */
        function();
    }
}