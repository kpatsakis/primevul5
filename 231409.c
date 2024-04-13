int trace_find_tgid(int pid)
{
	if (unlikely(!tgid_map || !pid || pid > PID_MAX_DEFAULT))
		return 0;

	return tgid_map[pid];
}