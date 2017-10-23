#pragma once


#include<iostream>
#include<string>
#include<queue>
#include<map>
#include<pthread.h>
#include <unistd.h>

using namespace std;

typedef void* LPVOID;

class MsgEntity {
public:
int message_type;
void* lparam;
void* wparam;
MsgEntity(){message_type=0;
lparam=0;
wparam=0;}
~MsgEntity(){}
};
class CMessageQueue;

typedef int(*Func_Ptr) (int, void*, void*);
typedef std::map<string, CMessageQueue*> map_ProcName_MsgQue;
typedef std::map<string, CMessageQueue*>::iterator Iter;

class CMessageQueue
{
//static
public:
static map_ProcName_MsgQue m_msgMap;
public:
static void PostMessage(const std::string& ProcName, int message_type, void* wparam, void* lparam) {
Iter iter = m_msgMap.find(ProcName);
if (iter == m_msgMap.end()) {
cout << "CMessageQueue::PostMessage ProcName not matched" << endl;
}
else {
CMessageQueue* que = iter->second;
que->PostMessage(message_type, wparam, lparam);
}
}



//subject
public:
CMessageQueue(std::string procName, Func_Ptr processor)
{
m_procName=procName;
m_func_ptr=processor;
m_tid=0;
m_msgMap[procName] = this;
pthread_mutexattr_init(&m_hMutexAttr);
pthread_mutexattr_settype(&m_hMutexAttr,PTHREAD_MUTEX_NORMAL);
pthread_mutex_init(&m_hMutex, &m_hMutexAttr);
}
~CMessageQueue() {
Iter iter = m_msgMap.find(m_procName);
if (iter != m_msgMap.end()) {
m_msgMap.erase(iter);
}
pthread_join(m_tid, NULL);
}
void start() {
pthread_create(&m_tid, NULL, CMessageQueue::execv, this);
}

private:
CMessageQueue() {
 m_tid=0;
//m_hMutex;
//m_hMutexAttr;
m_procName="";
m_func_ptr=0;
}

void PostMessage(int message_type, void* wparam, void* lparam)
{
if (m_func_ptr != NULL) {
MsgEntity msg;
msg.message_type = message_type;
msg.wparam = wparam;
msg.lparam = lparam;

pthread_mutex_lock(&m_hMutex);
m_msgQueue.push(msg);
pthread_mutex_unlock(&m_hMutex);
}
}
static void* execv(LPVOID param) {
cout << "start execv " << endl;
CMessageQueue * pThis = (CMessageQueue*)(param);
while (pThis->m_func_ptr != NULL) {
if (!pThis->m_msgQueue.empty())
{
MsgEntity msg;
pThis->HandleMsg(pThis->m_msgQueue, msg);
int message_type = msg.message_type;
void* wparam = msg.wparam;
void* lparam = msg.lparam;
pThis->m_func_ptr(message_type, wparam, lparam);
}
else {
usleep(5000);
}
}
cout << "exit" << endl;
return NULL;
}
void HandleMsg(queue<MsgEntity>& msgQueue, MsgEntity& msg)
{
pthread_mutex_lock(&m_hMutex);
msg = msgQueue.front();
m_msgQueue.pop();
pthread_mutex_unlock(&m_hMutex);
}
private:
//HANDLE m_hThread;
pthread_t m_tid;
//HANDLE m_hMutex;
pthread_mutex_t m_hMutex;
pthread_mutexattr_t m_hMutexAttr;
queue<MsgEntity> m_msgQueue;
std::string m_procName;
Func_Ptr m_func_ptr;
};
