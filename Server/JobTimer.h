#pragma once
#include "pch.h"
#include "Defines.h"
#include "JobQueue.h"

typedef struct myGreater {
	bool operator()(const pair<ULONGLONG, Job*> lhs, const pair<ULONGLONG, Job*> rhs) {
		return lhs.first > rhs.first;
	}
};

class JobTimer {
	SINGLETON(JobTimer);

public:
	void Register(std::function<void()> job, JobExecFreqType type, int interval = 0, int duration = 0);
	void OnUpdate();
	void Close();

private:
	priority_queue<pair<ULONGLONG, Job*>, vector<pair<ULONGLONG, Job*>>, myGreater> _pq;
	atomic<bool> _isClosed;
	mutex m_queue;
};

