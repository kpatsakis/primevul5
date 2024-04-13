
static int io_sq_thread(void *data)
{
	struct io_sq_data *sqd = data;
	struct io_ring_ctx *ctx;
	unsigned long timeout = 0;
	char buf[TASK_COMM_LEN];
	DEFINE_WAIT(wait);

	snprintf(buf, sizeof(buf), "iou-sqp-%d", sqd->task_pid);
	set_task_comm(current, buf);

	if (sqd->sq_cpu != -1)
		set_cpus_allowed_ptr(current, cpumask_of(sqd->sq_cpu));
	else
		set_cpus_allowed_ptr(current, cpu_online_mask);
	current->flags |= PF_NO_SETAFFINITY;

	mutex_lock(&sqd->lock);
	/* a user may had exited before the thread started */
	io_run_task_work_head(&sqd->park_task_work);

	while (!test_bit(IO_SQ_THREAD_SHOULD_STOP, &sqd->state)) {
		int ret;
		bool cap_entries, sqt_spin, needs_sched;

		if (test_bit(IO_SQ_THREAD_SHOULD_PARK, &sqd->state) ||
		    signal_pending(current)) {
			bool did_sig = false;

			mutex_unlock(&sqd->lock);
			if (signal_pending(current)) {
				struct ksignal ksig;

				did_sig = get_signal(&ksig);
			}
			cond_resched();
			mutex_lock(&sqd->lock);
			io_run_task_work();
			io_run_task_work_head(&sqd->park_task_work);
			if (did_sig)
				break;
			timeout = jiffies + sqd->sq_thread_idle;
			continue;
		}
		sqt_spin = false;
		cap_entries = !list_is_singular(&sqd->ctx_list);
		list_for_each_entry(ctx, &sqd->ctx_list, sqd_list) {
			const struct cred *creds = NULL;

			if (ctx->sq_creds != current_cred())
				creds = override_creds(ctx->sq_creds);
			ret = __io_sq_thread(ctx, cap_entries);
			if (creds)
				revert_creds(creds);
			if (!sqt_spin && (ret > 0 || !list_empty(&ctx->iopoll_list)))
				sqt_spin = true;
		}

		if (sqt_spin || !time_after(jiffies, timeout)) {
			io_run_task_work();
			cond_resched();
			if (sqt_spin)
				timeout = jiffies + sqd->sq_thread_idle;
			continue;
		}

		prepare_to_wait(&sqd->wait, &wait, TASK_INTERRUPTIBLE);
		if (!test_bit(IO_SQ_THREAD_SHOULD_PARK, &sqd->state)) {
			list_for_each_entry(ctx, &sqd->ctx_list, sqd_list)
				io_ring_set_wakeup_flag(ctx);

			needs_sched = true;
			list_for_each_entry(ctx, &sqd->ctx_list, sqd_list) {
				if ((ctx->flags & IORING_SETUP_IOPOLL) &&
				    !list_empty_careful(&ctx->iopoll_list)) {
					needs_sched = false;
					break;
				}
				if (io_sqring_entries(ctx)) {
					needs_sched = false;
					break;
				}
			}

			if (needs_sched) {
				mutex_unlock(&sqd->lock);
				schedule();
				mutex_lock(&sqd->lock);
			}
			list_for_each_entry(ctx, &sqd->ctx_list, sqd_list)
				io_ring_clear_wakeup_flag(ctx);
		}

		finish_wait(&sqd->wait, &wait);
		io_run_task_work_head(&sqd->park_task_work);
		timeout = jiffies + sqd->sq_thread_idle;
	}

	io_uring_cancel_sqpoll(sqd);
	sqd->thread = NULL;
	list_for_each_entry(ctx, &sqd->ctx_list, sqd_list)
		io_ring_set_wakeup_flag(ctx);
	io_run_task_work();
	io_run_task_work_head(&sqd->park_task_work);
	mutex_unlock(&sqd->lock);

	complete(&sqd->exited);
	do_exit(0);