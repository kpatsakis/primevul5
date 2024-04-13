static __init int register_snapshot_cmd(void)
{
	return register_ftrace_command(&ftrace_snapshot_cmd);
}