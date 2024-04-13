wrkr(void *myself)
{
	struct wrkrInfo_s *me = (struct wrkrInfo_s*) myself;
	pthread_mutex_lock(&io_q.mut);
	++wrkrRunning;
	pthread_mutex_unlock(&io_q.mut);

	io_req_t *n;
	while(1) {
		n = NULL;
		pthread_mutex_lock(&io_q.mut);
		if (io_q.sz == 0) {
			--wrkrRunning;
			if (glbl.GetGlobalInputTermState() != 0) {
				pthread_mutex_unlock(&io_q.mut);
				break;
			} else {
				pthread_cond_wait(&io_q.wakeup_worker, &io_q.mut);
			}
			++wrkrRunning;
		}
		if (io_q.sz > 0) {
			n = STAILQ_FIRST(&io_q.q);
			STAILQ_REMOVE_HEAD(&io_q.q, link);
			io_q.sz--;
		}
		pthread_mutex_unlock(&io_q.mut);

		if (n != NULL) {
			++me->numCalled;
			processWorkItem(n->epd);
			free(n);
		}
	}
	return NULL;
}