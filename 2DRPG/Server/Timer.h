#pragma once
class Timer
{
public:
	Timer();
	~Timer();

public:
	void TimerThread();
	void InitTimerQueue(TimerEvent ev);
	//void setIocpHandle(HANDLE handle) { _iocpHandle = handle; }
private:
	bool isRunning = false;
	thread _timerthread;
	concurrency::concurrent_priority_queue<TimerEvent> _timerqueue;
	std::mutex _TimerQueueLock;

};

