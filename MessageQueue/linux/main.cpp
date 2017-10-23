#include<iostream>
#include<pthread.h>
#include <unistd.h>
#include"MessageQueue.h"
using namespace std;

int Process(int msg_type,void* lparam,void* wparam)
{
cout<<"msg_type: "<<msg_type<<"lparam: "<<(*(int*)(lparam))
<<"wparam: "<<(*(double*)(wparam))<<"\n"<<endl;
return 0;
}

int main(int,char**)
{
const std::string proc_name_1="proc1";
const std::string proc_name_2="proc2";
const std::string proc_name_3="proc3";
int val_1=5;
double val_2=6.7;

CMessageQueue que_1(proc_name_1,Process);
CMessageQueue que_2(proc_name_2,Process);
CMessageQueue que_3(proc_name_3,Process);

que_1.start();
que_2.start();
que_3.start();

CMessageQueue::PostMessage(proc_name_1, 1,(void*)&val_1, (void*)&val_2);
CMessageQueue::PostMessage(proc_name_2, 2,(void*)&val_1, (void*)&val_2);
CMessageQueue::PostMessage(proc_name_3, 3,(void*)&val_1, (void*)&val_2);
CMessageQueue::PostMessage(proc_name_1, 4,(void*)&val_1, (void*)&val_2);
CMessageQueue::PostMessage(proc_name_2, 5,(void*)&val_1, (void*)&val_2);
CMessageQueue::PostMessage(proc_name_3, 6,(void*)&val_1, (void*)&val_2);


usleep(5000);
cout<<"ends\n"<<endl;
return 0;
}
