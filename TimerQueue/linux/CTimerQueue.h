#ifndef CTIMERQUEUE_H_
#define CTIMERQUEUE_H_
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<time.h>
#include<unistd.h>
#include<memory.h>
#include<iostream>
#include<map>

using std::map;

#define CLOCK_ID CLOCK_REALTIME

typedef int Key;
typedef void (*FuncPtr)(sigval v);

//void TimerProcFunc(sigval val);

class CTimer{
public:
	CTimer(){m_timer_t=0;}
	~CTimer(){}
	CTimer(Key m_timerID,FuncPtr funcPtr,long delay_ms){
		m_timer_t=0;
		memset(&m_evp,0,sizeof(m_evp));
		m_evp.sigev_value.sival_int=(int)m_timerID;
		m_evp.sigev_notify=SIGEV_THREAD;
		m_evp.sigev_notify_function=funcPtr;
		if(-1==timer_create(CLOCK_ID,&m_evp,&m_timer_t))
		{
			perror("failed to create timer\n");
		}
		m_delay.it_interval.tv_sec=10;
		m_delay.it_interval.tv_nsec=0;
		if(delay_ms>=1000)
		{
			m_delay.it_value.tv_sec=delay_ms/1000;
			m_delay.it_value.tv_nsec=0;
		}
		else
		{
			m_delay.it_value.tv_sec=0;
			m_delay.it_value.tv_nsec=delay_ms*1000000;
		}
	}
	void startTimer(){
		if(-1==timer_settime(m_timer_t,0,&m_delay,NULL))
		{
			perror("failed to start timer\n");
		}
	}
	void killTimer(){
		timer_delete(m_timer_t);
	}
private:
	timer_t  m_timer_t;
	sigevent m_evp;
	itimerspec m_delay;

};

class CTimerQueue {
public:
	CTimerQueue(){
	}
	~CTimerQueue(){}
	Key addTimer(FuncPtr funcPtr,long delay_ms){
		Key timer_ID=++m_current_timerID;
		CTimer timer(timer_ID,funcPtr,delay_ms);
		if(m_timers.find(timer_ID)!=m_timers.end())
		{
			perror("timer existed\n");
		}
		else
		{
			m_timers[timer_ID]=timer;
			timer.startTimer();
		}
		return timer_ID;
	}
	void killTimer(Key timer_ID)
	{
		map<Key,CTimer>::iterator iter=m_timers.find(timer_ID);
		if(iter!=m_timers.end())
		{
			CTimer timer=iter->second;
			timer.killTimer();
			m_timers.erase(iter);
		}
	}
	static int m_current_timerID;
private:
	map<Key,CTimer> m_timers;
};

#endif /* CTIMERQUEUE_H_ */
