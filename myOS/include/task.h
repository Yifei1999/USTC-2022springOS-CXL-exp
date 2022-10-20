#ifndef __TASK_H__
#define __TASK_H__

#define NULL ((void*)0)
#define TASK_NUM 10
#define STACK_SIZE 0x800
#define NULL ((void*)0)

#define PRIORITY 1
#define EXETIME 2
#define ARRTIME 3


typedef struct tskPara
{
	// priority: 0 for most prior, 99 for idle
	// exeTime: task's left run time: unit "s"
	// arrTime: the time that task due to arrive
	unsigned long priority;
	unsigned long exeTime;
	unsigned long arrTime;
} tskPara;


typedef struct myTCB
{
	unsigned long tid;
    unsigned long *stackTop;
	void (*taskBody)(void);
    struct myTCB *next; // ready
	struct myTCB *nextarr;
	unsigned long status;  
	// 1: new
	// 2: ready
	// 3: running
	// 4: waiting
	// 5: terminated

	tskPara infor;
} myTCB; 


extern unsigned long taskNum;
extern myTCB taskPool[20];
extern unsigned long **prevTSK_StackPtrAddr;
extern unsigned long *nextTSK_StackPtr;


void initTskPara(tskPara *buffer);
void setTskPara(unsigned int option, unsigned int value, tskPara *buffer);
void getTskPara(unsigned int option, unsigned int *para, myTCB *buffer);


void startMultitask(void);
int createTsk(void (*tskBody)(void), tskPara *otherInf);
void tskStart(myTCB *tsk);
void tskEnd();
void destroyTsk(int tskIndex);


void task1Body();
void task2Body();
void task3Body();
void task4Body();
void task5Body();
void taskIdleBody();


void context_switch(unsigned long **prevTskStkAddr, unsigned long *nextTskStk);

#endif