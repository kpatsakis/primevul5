static inline void ftrace_exports_enable(void)
{
	static_branch_enable(&ftrace_exports_enabled);
}