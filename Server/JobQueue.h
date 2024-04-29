#pragma once
#include "pch.h"
#include "Defines.h"

#include "Job.h"

class JobQueue {
	SINGLETON(JobQueue);

public:
	void Push(Job job);
	void Fetch();
	void Close();

private:
	vector<Job> _jobsPushed;
	vector<Job> _jobsToFetch;
	mutex m_push, m_fetch;
	atomic<bool> _isClosed;
};

