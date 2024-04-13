static bool io_match_task(struct io_kiocb *head,
			  struct task_struct *task,
			  struct files_struct *files)
{
	struct io_kiocb *req;

	if (task && head->task != task)
		return false;
	if (!files)
		return true;

	io_for_each_link(req, head) {
		if (req->flags & REQ_F_INFLIGHT)
			return true;
	}
	return false;
}