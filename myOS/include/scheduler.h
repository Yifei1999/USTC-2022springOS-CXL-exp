#ifndef __SCHEDULER_H__
#define __SCHEDULER__

#include "./task.h"

myTCB *nextTsk(void);    // 根据调度规则获取下一个调度的任务
void enqueueTsk(myTCB *tsk);  // 到达队列
void dequeueTsk();

void schedulerInit();   // 对调度器做设置
void createTsk_hook(myTCB *created);   // 创建任务的时候完成
void scheduler_tick(void); // run every tick
void schedule();


typedef struct scheduler {
	unsigned int type;   // 调度器类型
	myTCB* (*nextTsk_func)(void);    //
	void (*enqueueTsk_func)(myTCB *tsk);
	void (*dequeueTsk_func)(void);
	void (*schedulerInit_func)(void);
	void (*createTsk_hook)(); //if set, will be call in createTsk (before tskStart)
	void (*tick_hook)(void); //if set, tick_hook will be called every tick
} scheduler;


extern scheduler scheduleInUse;
extern myTCB *arrivedQueue;
extern myTCB *readyQueue;

#endif