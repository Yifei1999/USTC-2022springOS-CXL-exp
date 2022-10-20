#include "../include/scheduler.h"
#include "../include/task.h"



extern scheduler task_FIFO;


myTCB* nextTsk_FIFO(void)
{
    return readyQueue->next;
}


void enqueueTsk_FIFO(myTCB *tsk)
{

    myTCB *cur = arrivedQueue;
    if (arrivedQueue == NULL)
    {
        arrivedQueue = tsk;
        tsk->nextarr = NULL;
    }
    else
    {
        if (cur->infor.arrTime > tsk->infor.arrTime)
        {
            tsk->nextarr = cur;
            arrivedQueue = tsk;  
        }
        else
        {    
            while ( cur->nextarr != NULL && cur->nextarr->infor.arrTime <= tsk->infor.arrTime)
            {
                cur = cur->nextarr;
            }
            tsk->nextarr = cur->nextarr;
            cur->nextarr = tsk;
        }
    }
}

void dequeueTsk_FIFO()
{
    if (arrivedQueue != NULL)
    {
        arrivedQueue = arrivedQueue->nextarr;
    }
}

extern void (*tick_hook)(void); 
void schedulerInit_FIFO(void)
{
    tick_hook = task_FIFO.tick_hook;
}

	
void createTsk_hook_FIFO()
{
    // tskPara scheduleInfor;
    // initTskPara(&scheduleInfor);
    // setTskPara(PRIORITY, 99, &scheduleInfor);
    // setTskPara(ARRTIME, 0, &scheduleInfor);
    // setTskPara(EXETIME, 0xffffffff, &scheduleInfor);
    
    // taskPool[taskNum].infor = scheduleInfor;
    // taskPool[taskNum].status = 1;
    // taskPool[taskNum].tid = taskNum;
    // taskPool[taskNum].taskBody = taskIdleBody;
    // taskNum ++;

    // unsigned long tmp_id = taskPool[taskNum-1].tid;
    // tskStart(&taskPool[tmp_id]);
}


extern int tick_number;   // 100HZ
void tick_hook_FIFO(void)
{
    
    while ( (arrivedQueue != NULL) && ( (arrivedQueue->infor).arrTime <= tick_number/100) )
    {
        
        tskStart(arrivedQueue);
        dequeueTsk_FIFO();
    }
}



scheduler task_FIFO = {1,
            nextTsk_FIFO,
            enqueueTsk_FIFO,
            dequeueTsk_FIFO,
            schedulerInit_FIFO,
            createTsk_hook_FIFO,
            tick_hook_FIFO
            };