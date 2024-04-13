static void io_task_work_add_head(struct callback_head **work_head,
				  struct callback_head *task_work)
{
	struct callback_head *head;

	do {
		head = READ_ONCE(*work_head);
		task_work->next = head;
	} while (cmpxchg(work_head, head, task_work) != head);
}