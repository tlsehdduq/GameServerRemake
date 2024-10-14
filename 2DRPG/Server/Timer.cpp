#include"stdafx.h"
#include "Timer.h"

Timer::Timer()
{
	isRunning = true;
	_timerthread = std::thread{ [&]() {TimerThread(); } };
}

Timer::~Timer()
{
	isRunning = false;
	if (_timerthread.joinable())
		_timerthread.join();
}

void Timer::TimerThread()
{
	while (isRunning)
	{
		TimerEvent ev;
		auto cur_time = std::chrono::system_clock::now();
		if (!_timerqueue.empty())
		{
			if (true == _timerqueue.try_pop(ev))
			{
				if (ev.wakeupTime > cur_time)
				{
					this_thread::sleep_for(30ms);
					continue;
				}
				switch (ev.evtype)
				{
				case EVENT_TYPE::EV_NPC_MOVE: {
					OVERLAPPED_EX* ov = new OVERLAPPED_EX;
					ov->_type = COMP_TYPE::NPC_UPDATE;
					ov->target_id = ev.n_id;
					PostQueuedCompletionStatus(_iocphandle, 1, ev.c_id, &ov->_over);
					break;
				}
				}
			}
		}
	}
}

void Timer::InitTimerQueue(TimerEvent ev)
{
	std::lock_guard<std::mutex> timerlockguard{ _TimerQueueLock };
	_timerqueue.push(ev);
}
