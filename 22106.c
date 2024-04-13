destroyIoQ(void)
{
	io_req_t *n;
	if (io_q.stats != NULL) {
		statsobj.Destruct(&io_q.stats);
	}
	pthread_mutex_lock(&io_q.mut);
	while (!STAILQ_EMPTY(&io_q.q)) {
		n = STAILQ_FIRST(&io_q.q);
		STAILQ_REMOVE_HEAD(&io_q.q, link);
		errmsg.LogError(0, RS_RET_INTERNAL_ERROR, "imptcp: discarded enqueued io-work to allow shutdown - ignored");
		free(n);
	}
	io_q.sz = 0;
	pthread_mutex_unlock(&io_q.mut);
	pthread_cond_destroy(&io_q.wakeup_worker);
	pthread_mutex_destroy(&io_q.mut);
}