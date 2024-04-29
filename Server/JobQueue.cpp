#include "pch.h"
#include "JobQueue.h"

JobQueue::JobQueue() {
	_isClosed = false;
}

JobQueue::~JobQueue() {
	
	//����� �۾��� ����
	{
		lock_guard<mutex> guard(m_push);

		while (_jobsPushed.empty() == false) {
			_jobsPushed.pop_back();
		}
	}

	//������ �۾��� ����
	{
		lock_guard<mutex> guard(m_fetch);

		while (_jobsToFetch.empty() == false) {
			_jobsToFetch.pop_back();
		}
	}
}

void JobQueue::Push(Job job) {
	lock_guard<mutex> guard(m_push);

	_jobsPushed.push_back(job);
}

void JobQueue::Fetch() {
	while (!_isClosed) {
		{
			lock_guard<mutex> guard(m_fetch);

			while (_jobsToFetch.empty() == false) {
				Job job = _jobsToFetch.back();
				_jobsToFetch.pop_back();
				job._job();
			}
		}
		
		if (_jobsPushed.empty() == false) {
			lock_guard<mutex> guard(m_push);

			while (_jobsPushed.empty() == false) {
				_jobsToFetch.push_back(_jobsPushed.back());
				_jobsPushed.pop_back();
			}

			_jobsPushed.clear();
		}
	}
}

void JobQueue::Close() { _isClosed = true; }
