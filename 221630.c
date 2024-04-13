
static bool io_cancel_defer_files(struct io_ring_ctx *ctx,
				  struct task_struct *task,
				  struct files_struct *files)
{
	struct io_defer_entry *de;
	LIST_HEAD(list);

	spin_lock_irq(&ctx->completion_lock);
	list_for_each_entry_reverse(de, &ctx->defer_list, list) {
		if (io_match_task(de->req, task, files)) {
			list_cut_position(&list, &ctx->defer_list, &de->list);
			break;
		}
	}
	spin_unlock_irq(&ctx->completion_lock);
	if (list_empty(&list))
		return false;

	while (!list_empty(&list)) {
		de = list_first_entry(&list, struct io_defer_entry, list);
		list_del_init(&de->list);
		io_req_complete_failed(de->req, -ECANCELED);
		kfree(de);
	}
	return true;