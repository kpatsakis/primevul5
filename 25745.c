compress_worker_thread_func(void *arg)
{
	comp_thread_ctxt_t *thd = (comp_thread_ctxt_t *) arg;

	pthread_mutex_lock(&thd->ctrl_mutex);

	pthread_mutex_lock(&thd->data_mutex);

	thd->started = TRUE;
	pthread_cond_signal(&thd->ctrl_cond);

	pthread_mutex_unlock(&thd->ctrl_mutex);

	while (1) {
		thd->data_avail = FALSE;
		pthread_cond_signal(&thd->data_cond);

		while (!thd->data_avail && !thd->cancelled) {
			pthread_cond_wait(&thd->data_cond, &thd->data_mutex);
		}

		if (thd->cancelled)
			break;

		thd->to_len = qlz_compress(thd->from, thd->to, thd->from_len,
					   &thd->state);

		/* qpress uses 0x00010000 as the initial value, but its own
		Adler-32 implementation treats the value differently:
		  1. higher order bits are the sum of all bytes in the sequence
		  2. lower order bits are the sum of resulting values at every
		     step.
		So it's the other way around as compared to zlib's adler32().
		That's why  0x00000001 is being passed here to be compatible
		with qpress implementation. */

		thd->adler = adler32(0x00000001, (uchar *) thd->to,
				     (uInt)thd->to_len);
	}

	pthread_mutex_unlock(&thd->data_mutex);

	return NULL;
}