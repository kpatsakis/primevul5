	__releases(ctx->uring_lock)
	__acquires(ctx->uring_lock)
{
	int ret;

	/*
	 * We're inside the ring mutex, if the ref is already dying, then
	 * someone else killed the ctx or is already going through
	 * io_uring_register().
	 */
	if (percpu_ref_is_dying(&ctx->refs))
		return -ENXIO;

	if (ctx->restricted) {
		if (opcode >= IORING_REGISTER_LAST)
			return -EINVAL;
		opcode = array_index_nospec(opcode, IORING_REGISTER_LAST);
		if (!test_bit(opcode, ctx->restrictions.register_op))
			return -EACCES;
	}

	if (io_register_op_must_quiesce(opcode)) {
		percpu_ref_kill(&ctx->refs);

		/*
		 * Drop uring mutex before waiting for references to exit. If
		 * another thread is currently inside io_uring_enter() it might
		 * need to grab the uring_lock to make progress. If we hold it
		 * here across the drain wait, then we can deadlock. It's safe
		 * to drop the mutex here, since no new references will come in
		 * after we've killed the percpu ref.
		 */
		mutex_unlock(&ctx->uring_lock);
		do {
			ret = wait_for_completion_interruptible(&ctx->ref_comp);
			if (!ret)
				break;
			ret = io_run_task_work_sig();
			if (ret < 0)
				break;
		} while (1);
		mutex_lock(&ctx->uring_lock);

		if (ret) {
			io_refs_resurrect(&ctx->refs, &ctx->ref_comp);
			return ret;
		}
	}

	switch (opcode) {
	case IORING_REGISTER_BUFFERS:
		ret = io_sqe_buffers_register(ctx, arg, nr_args, NULL);
		break;
	case IORING_UNREGISTER_BUFFERS:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_sqe_buffers_unregister(ctx);
		break;
	case IORING_REGISTER_FILES:
		ret = io_sqe_files_register(ctx, arg, nr_args, NULL);
		break;
	case IORING_UNREGISTER_FILES:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_sqe_files_unregister(ctx);
		break;
	case IORING_REGISTER_FILES_UPDATE:
		ret = io_register_files_update(ctx, arg, nr_args);
		break;
	case IORING_REGISTER_EVENTFD:
	case IORING_REGISTER_EVENTFD_ASYNC:
		ret = -EINVAL;
		if (nr_args != 1)
			break;
		ret = io_eventfd_register(ctx, arg);
		if (ret)
			break;
		if (opcode == IORING_REGISTER_EVENTFD_ASYNC)
			ctx->eventfd_async = 1;
		else
			ctx->eventfd_async = 0;
		break;
	case IORING_UNREGISTER_EVENTFD:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_eventfd_unregister(ctx);
		break;
	case IORING_REGISTER_PROBE:
		ret = -EINVAL;
		if (!arg || nr_args > 256)
			break;
		ret = io_probe(ctx, arg, nr_args);
		break;
	case IORING_REGISTER_PERSONALITY:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_register_personality(ctx);
		break;
	case IORING_UNREGISTER_PERSONALITY:
		ret = -EINVAL;
		if (arg)
			break;
		ret = io_unregister_personality(ctx, nr_args);
		break;
	case IORING_REGISTER_ENABLE_RINGS:
		ret = -EINVAL;
		if (arg || nr_args)
			break;
		ret = io_register_enable_rings(ctx);
		break;
	case IORING_REGISTER_RESTRICTIONS:
		ret = io_register_restrictions(ctx, arg, nr_args);
		break;
	case IORING_REGISTER_RSRC:
		ret = io_register_rsrc(ctx, arg, nr_args);
		break;
	case IORING_REGISTER_RSRC_UPDATE:
		ret = io_register_rsrc_update(ctx, arg, nr_args);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	if (io_register_op_must_quiesce(opcode)) {
		/* bring the ctx back to life */
		percpu_ref_reinit(&ctx->refs);
		reinit_completion(&ctx->ref_comp);
	}
	return ret;