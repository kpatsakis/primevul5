static int tracing_wait_pipe(struct file *filp)
{
	struct trace_iterator *iter = filp->private_data;
	int ret;

	while (trace_empty(iter)) {

		if ((filp->f_flags & O_NONBLOCK)) {
			return -EAGAIN;
		}

		/*
		 * We block until we read something and tracing is disabled.
		 * We still block if tracing is disabled, but we have never
		 * read anything. This allows a user to cat this file, and
		 * then enable tracing. But after we have read something,
		 * we give an EOF when tracing is again disabled.
		 *
		 * iter->pos will be 0 if we haven't read anything.
		 */
		if (!tracer_tracing_is_on(iter->tr) && iter->pos)
			break;

		mutex_unlock(&iter->mutex);

		ret = wait_on_pipe(iter, false);

		mutex_lock(&iter->mutex);

		if (ret)
			return ret;
	}

	return 1;
}