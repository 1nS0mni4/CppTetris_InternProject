#include "pch.h"
#include "Job.h"

Job::Job(std::function<void()> job, JobExecFreqType type, int interval, int duration)
	: _job(job),
	_execType(type),
	_duration(duration),
	_interval(interval)
{

}