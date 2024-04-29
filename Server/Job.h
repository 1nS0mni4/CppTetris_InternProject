#pragma once
#include "pch.h"

enum class JobExecFreqType : USHORT{
	ONCE,
	CERTAIN,
	PERMANENT,
};

class Job {
	friend class JobQueue;
	friend class JobTimer;

public:
	Job(std::function<void()> job, JobExecFreqType type = JobExecFreqType::ONCE, int interval = 0, int duration = 0);

private:
	std::function<void()> _job;
	JobExecFreqType _execType;
	int _duration;
	int _interval;
};

