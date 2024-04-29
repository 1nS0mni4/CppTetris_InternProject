#include "pch.h"
#include "Job.h"
#include "JobTimer.h"

JobTimer::JobTimer() {
	_isClosed = false;
}

JobTimer::~JobTimer() {
	lock_guard<mutex> guard(m_queue);

	while (_pq.empty() == false) {
		pair<int, Job*> reserved = _pq.top();
		delete reserved.second;
		_pq.pop();
	}
}

void JobTimer::Register(std::function<void()> lambda, JobExecFreqType type, int interval, int duration) {
	if (type == JobExecFreqType::CERTAIN && duration <= 0)
		return;

	Job* reservation = new Job(lambda, type, interval, duration);

	lock_guard<mutex> guard(m_queue);
	_pq.push(make_pair((ULONGLONG)(GetTickCount64() + interval * 1000), reservation));
}

void JobTimer::OnUpdate() {
	while (_isClosed == false) {
		int sleepMs = 0;

		while (true) {
			pair<ULONGLONG, Job*> reserved;
			ULONGLONG curTick = GetTickCount64();
			if (_pq.empty())
				break;
			{
				lock_guard<mutex> guard(m_queue);
				reserved = _pq.top();

				if (reserved.first >= curTick) {
					sleepMs = reserved.first - curTick;
					break;
				}
				_pq.pop();
			}

			Job* job = reserved.second;
			JobQueue::GetInstance().Push(*(reserved.second));

			switch (reserved.second->_execType) {
				case JobExecFreqType::CERTAIN:
				{
					if (job->_duration >= 1) {
						job->_duration--;
						lock_guard<mutex> guard(m_queue);
						_pq.push(make_pair((ULONGLONG)(GetTickCount64() + job->_interval * 1000), job));
						break;
					}
				}
				case JobExecFreqType::ONCE:
				{
					delete job;
					break;
				}
				case JobExecFreqType::PERMANENT:
				{
					lock_guard<mutex> guard(m_queue);
					_pq.push(make_pair((ULONGLONG)(GetTickCount64() + job->_interval * 1000), job));
					break;
				}

				default: break;
			}
		}
		
		Sleep(sleepMs);
	}
}

void JobTimer::Close() { _isClosed = true; }
