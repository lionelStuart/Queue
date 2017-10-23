#pragma once
#include<windows.h>
#include<iostream>
#include<string>
#include<queue>
#include<map>

//using std::queue;
//using std::string;
//using std::map;
using namespace std;

struct MsgEntity {
int message_type;
void* lparam;
void* wparam;
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
CMessageQueue(std::string procName, Func_Ptr processor) :m_procName(procName),
m_func_ptr(processor), m_hThread(0) {
m_msgMap[procName] = this;
m_hMutex = CreateMutex(NULL, false, NULL);
}
~CMessageQueue() {
if (m_hThread) {
CloseHandle(m_hThread);
}
CloseHandle(m_hMutex);
Iter iter = m_msgMap.find(m_procName);
if (iter != m_msgMap.end()) {
m_msgMap.erase(iter);
}
}
void start() {
if (!m_hThread)
m_hThread = CreateThread(NULL, 0, CMessageQueue::execv, LPVOID(this), 0, NULL);
}
private:
CMessageQueue() {}
void PostMessage(int message_type, void* wparam, void* lparam)
{
if (m_func_ptr != NULL) {
MsgEntity msg;
msg.message_type = message_type;
msg.wparam = wparam;
msg.lparam = lparam;

WaitForSingleObject(m_hMutex, INFINITE);
m_msgQueue.push(msg);
ReleaseMutex(m_hMutex);
}
}
static DWORD WINAPI execv(LPVOID param) {
cout << "start execv" << endl;
CMessageQueue * pThis = (CMessageQueue*)(param);
while (pThis->m_func_ptr != NULL) {
if (!pThis->m_msgQueue.empty())
{
//cout << "\nrunning out" << endl;
MsgEntity msg;
pThis->HandleMsg(pThis->m_msgQueue, msg);
int message_type = msg.message_type;
void* wparam = msg.wparam;
void* lparam = msg.lparam;
pThis->m_func_ptr(message_type, wparam, lparam);
}
else {
//cout << "empty" << endl;
Sleep(2);
}
}
cout << "exit" << endl;
return NULL;
}
void HandleMsg(queue<MsgEntity>& msgQueue, MsgEntity& msg)
{
WaitForSingleObject(m_hMutex, INFINITE);
msg = msgQueue.front();
m_msgQueue.pop();
ReleaseMutex(m_hMutex);
}
private:
HANDLE m_hThread;
HANDLE m_hMutex;
queue<MsgEntity> m_msgQueue;
std::string m_procName;
Func_Ptr m_func_ptr;
};

#pragma once
#include "stdafx.h"
#include <set>
class CTimerQueue
{
public:
CTimerQueue() {
m_hTimerQueue = CreateTimerQueue();
}
HANDLE getTimerQueue() {
return m_hTimerQueue;
}
bool addTimer(HANDLE& hTimer,DWORD dueTime, WAITORTIMERCALLBACK callBack, PVOID& wParam) {
//HANDLE hTimer;
if (CreateTimerQueueTimer(&hTimer, m_hTimerQueue, callBack, wParam, dueTime, 0, 0)) {
m_hTimers.insert(hTimer);
return true;
}
return false;
}
bool KillTimer(HANDLE &hTimer) {
std::set<HANDLE>::iterator iter = m_hTimers.find(hTimer);
if (iter != m_hTimers.end()) {
DeleteTimerQueueTimer(m_hTimerQueue, *iter, NULL);
m_hTimers.erase(iter);
}
return true;
}
~CTimerQueue() {
DeleteTimerQueue(m_hTimerQueue);
m_hTimers.clear();
}
std::set<HANDLE> m_hTimers;
HANDLE m_hTimerQueue;

};
