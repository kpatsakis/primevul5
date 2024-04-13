static void __trace_find_cmdline(int pid, char comm[])
{
	unsigned map;

	if (!pid) {
		strcpy(comm, "<idle>");
		return;
	}

	if (WARN_ON_ONCE(pid < 0)) {
		strcpy(comm, "<XXX>");
		return;
	}

	if (pid > PID_MAX_DEFAULT) {
		strcpy(comm, "<...>");
		return;
	}

	map = savedcmd->map_pid_to_cmdline[pid];
	if (map != NO_CMDLINE_MAP)
		strlcpy(comm, get_saved_cmdlines(map), TASK_COMM_LEN);
	else
		strcpy(comm, "<...>");
}