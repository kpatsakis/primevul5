static inline void set_cmdline(int idx, const char *cmdline)
{
	memcpy(get_saved_cmdlines(idx), cmdline, TASK_COMM_LEN);
}