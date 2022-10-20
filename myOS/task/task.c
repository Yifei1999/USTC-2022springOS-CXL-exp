#include "../include/task.h"
#include "../include/scheduler.h"

#include "../include/malloc.h"
#include "../include/myPrintk.h"

unsigned long taskNum = 0;
myTCB taskPool[20];

unsigned long BspContextBase[STACK_SIZE];
unsigned long *BspContext;
void startMultitask(void) 
{
	BspContext = BspContextBase + STACK_SIZE - 1;
	myTCB *firstTsk = readyQueue;

	context_switch(&BspContext, firstTsk->stackTop);
}

void initTskPara(tskPara *buffer)
{   
	// set default value 
    buffer->arrTime = 0;
	buffer->exeTime = 1;
	buffer->priority = 3;
	return;
}

void setTskPara(unsigned int option, unsigned int value, tskPara *buffer)
{
    unsigned long *dataPtr = (unsigned long*)buffer;
	*(dataPtr - 1 + option) = value;
	return;
}

void getTskPara(unsigned int option, unsigned int *para, myTCB *tsk)
{
    unsigned long *dataPtr = (unsigned long*)(&tsk->infor);
	*para = *(dataPtr - 1 + option);
	return;
}

/**
 * create a task
 * tskBody: the pointer of task entrance function 
 *  
 * return: the PID of task
 */
int createTsk(void (*tskBody)(void),tskPara *otherInf)
{

	taskPool[taskNum].status = 1;
	taskPool[taskNum].tid = taskNum;
	taskPool[taskNum].taskBody = tskBody;
	taskPool[taskNum].infor = *otherInf;
    
	enqueueTsk(&taskPool[taskNum]);

	taskNum ++;
    
	return taskPool[taskNum-1].tid;
}


void tskStart(myTCB *tsk)
{ // 设置TCB为就绪，stack init
    tsk->status = 2;
    unsigned long st = (unsigned long *)malloc(STACK_SIZE);
    tsk->stackTop = st + STACK_SIZE - 4;
	stack_init( &(tsk->stackTop), tsk->taskBody); 

	// 插入到就绪队列
	myTCB *cur = readyQueue;
	
    if (cur == NULL)
	{
        readyQueue = tsk;
		tsk->next = NULL;
	}
    else
	{
	
		while ( (cur->next) != NULL)
	    {
		
		    cur = cur->next;
	    }
	
	    cur->next = tsk;
	    tsk->next = NULL;
	
	}

	return;
}


void tskEnd()
{
	// 结束任务, 移除出就绪队列，通知操作系统进行下一个任务
    // 手动添加在每个函数的最后,来结束任务的运行
    // 通过schedule函数调度进程

	readyQueue->status = 5;
    schedule();
	return;
}


void destroyTsk(int tskIndex)
/**
 * destroy a task 
 * 如何表示一个被摧毁的进程？
 */
{
	free(taskPool[tskIndex].stackTop);
    return;
}


unsigned long **prevTSK_StackPtrAddr;
unsigned long *nextTSK_StackPtr;
extern void CTX_SW(void);
void context_switch(unsigned long **prevTskStkAddr, unsigned long *nextTskStk) 
{
	prevTSK_StackPtrAddr = prevTskStkAddr;
	nextTSK_StackPtr = nextTskStk;
	CTX_SW();
}

void stack_init(unsigned long **stk, void (*task)(void)) 
{// 初始化任务，建立堆栈
	*(*stk)-- = (unsigned long)0x08; // CS
	*(*stk)-- = (unsigned long)task; // eip
	
	// pushf
	*(*stk)-- = (unsigned long)0x0202; // flag registers
	// pusha

	*(*stk)-- = (unsigned long)0xAAAAAAAA; // eax
	*(*stk)-- = (unsigned long)0xCCCCCCCC; // ecx
	*(*stk)-- = (unsigned long)0xDDDDDDDD; // edx
	*(*stk)-- = (unsigned long)0xBBBBBBBB; // ebx
	*(*stk)-- = (unsigned long)0x44444444; // esp
	*(*stk)-- = (unsigned long)0x55555555; // ebp
	*(*stk)-- = (unsigned long)0x66666666; // esp
	**stk = (unsigned long)0x77777777; // edi
	
}

extern int tick_number, hh, mm, ss;
void task1Body()
{
    myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*     this is task1 start\n");
	myPrintk(0x5,"*\n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
	myPrintk(0x5,"****************************\n\n");
    int startTime = tick_number;
	while (taskPool[2].infor.exeTime >= 1)
	{
		
	}
    myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*       task1 end\n");
	myPrintk(0x5,"* \n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
    myPrintk(0x5,"****************************\n\n");

	tskEnd();
	return;
}

void task2Body()
{
    myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*     this is task2 start\n");
	myPrintk(0x5,"*\n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
	myPrintk(0x5,"****************************\n\n");
    int startTime = tick_number;
	while (taskPool[3].infor.exeTime >= 1)
	{
		
	}
	myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*       task2 end\n");
	myPrintk(0x5,"* \n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
    myPrintk(0x5,"****************************\n\n");

	tskEnd();
	return;
}

void task3Body()
{
    myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*     this is task3 start\n");
	myPrintk(0x5,"*\n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
	myPrintk(0x5,"****************************\n\n");
    int startTime = tick_number;
	while (taskPool[4].infor.exeTime >= 1)
	{
		
	}
	myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*       task3 end\n");
	myPrintk(0x5,"* \n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
    myPrintk(0x5,"****************************\n\n");

	tskEnd();
	return;
}

void task4Body()
{
    myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*     this is task4 start\n");
	myPrintk(0x5,"*\n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
	myPrintk(0x5,"****************************\n\n");
    int startTime = tick_number;
	while (taskPool[5].infor.exeTime >= 1)
	{
		
	}
	myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*       task4 end\n");
	myPrintk(0x5,"* \n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
    myPrintk(0x5,"****************************\n\n");
	tskEnd();
	return;
}


void task5Body()
{
    myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*     this is task5 start\n");
	myPrintk(0x5,"*\n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
	myPrintk(0x5,"****************************\n\n");
	
    int startTime = tick_number;
	while (taskPool[6].infor.exeTime >= 1)
	{
		
	}
	myPrintk(0x5,"****************************\n");
	myPrintk(0x5,"*       task5 end\n");
	myPrintk(0x5,"* \n");
	myPrintk(0x5,"* @liuyifei time_stamp: %d:%d:%d\n",hh,mm,ss);
    myPrintk(0x5,"****************************\n\n");

	tskEnd();
	return;
}

extern scheduler scheduleInUse;
void taskIdleBody()
{

	while (1)
	{
		if (readyQueue->next != NULL)  // find a new ready task
		{
			// if we unable preempt function, the following code block need to be executed...
            // ready a new idle task to the ready queue
            
			{
				// scheduleInUse.createTsk_hook();
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
			tskEnd();
		}	
	}
}

