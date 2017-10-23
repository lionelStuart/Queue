#include<iostream>
#include<pthread.h>
#include "CTimerQueue.h"

using namespace std;

class App
{
public:
	static App* ptrAppFunc;
	 static void TimerProcFunc(sigval val)
	{
		pthread_mutex_lock(&ptrAppFunc->m_mutex);
		Key timerID=(Key)val.sival_int;
		(ptrAppFunc->m_tq).killTimer(timerID);
		pthread_mutex_unlock(&ptrAppFunc->m_mutex);
		//TODO

		printf("get timer ID%d\n",timerID);

	}
	void addTimer()
	{
		m_tq.addTimer(App::TimerProcFunc,1000);
		m_tq.addTimer(App::TimerProcFunc,2000);
		m_tq.addTimer(App::TimerProcFunc,3000);
		m_tq.addTimer(App::TimerProcFunc,1000);
		m_tq.addTimer(App::TimerProcFunc,2000);
		m_tq.addTimer(App::TimerProcFunc,3000);
		m_tq.addTimer(App::TimerProcFunc,5000);
	}
	App()
	{
		pthread_mutex_init(&m_mutex,NULL);
		ptrAppFunc=this;
	}
private:
	CTimerQueue m_tq;
	pthread_mutex_t m_mutex;
};
App* App::ptrAppFunc=NULL;




int main(int,char**)
{

	App app;
	app.addTimer();

	while(true)
	sleep(2);

	cout<<"ends\n"<<endl;
	return 0;
}

