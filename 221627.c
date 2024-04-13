static bool io_run_task_work_head(struct callback_head **work_head)
{
	struct callback_head *work, *next;
	bool executed = false;

	do {
		work = xchg(work_head, NULL);
		if (!work)
			break;

		do {
			next = work->next;
			work->func(work);
			work = next;
			cond_resched();
		} while (work);
		executed = true;
	} while (1);

	return executed;
}