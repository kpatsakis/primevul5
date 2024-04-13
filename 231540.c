static inline void ftrace_exports_disable(void)
{
	static_branch_disable(&ftrace_exports_enabled);
}