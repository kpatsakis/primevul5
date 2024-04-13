char *__get_task_comm(char *buf, size_t buf_size, struct task_struct *tsk)
{
	task_lock(tsk);
	strncpy(buf, tsk->comm, buf_size);
	task_unlock(tsk);
	return buf;
}