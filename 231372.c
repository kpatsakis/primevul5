static bool tracing_record_taskinfo_skip(int flags)
{
	if (unlikely(!(flags & (TRACE_RECORD_CMDLINE | TRACE_RECORD_TGID))))
		return true;
	if (atomic_read(&trace_record_taskinfo_disabled) || !tracing_is_on())
		return true;
	if (!__this_cpu_read(trace_taskinfo_save))
		return true;
	return false;
}