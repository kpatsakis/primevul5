u64 ftrace_now(int cpu)
{
	return buffer_ftrace_now(&global_trace.trace_buffer, cpu);
}