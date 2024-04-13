static inline char *get_saved_cmdlines(int idx)
{
	return &savedcmd->saved_cmdlines[idx * TASK_COMM_LEN];
}