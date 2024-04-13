enqueueIoWork(epolld_t *epd, int dispatchInlineIfQueueFull) {
	io_req_t *n;
	int dispatchInline;
	DEFiRet;
	
	CHKmalloc(n = malloc(sizeof(io_req_t)));
	n->epd = epd;
	
	int inlineDispatchThreshold = DFLT_inlineDispatchThreshold * runModConf->wrkrMax;
	dispatchInline = 0;
	
	pthread_mutex_lock(&io_q.mut);
	if (dispatchInlineIfQueueFull && io_q.sz > inlineDispatchThreshold) {
		dispatchInline = 1;
	} else {
		STAILQ_INSERT_TAIL(&io_q.q, n, link);
		io_q.sz++;
		STATSCOUNTER_INC(io_q.ctrEnq, io_q.mutCtrEnq);
		STATSCOUNTER_SETMAX_NOMUT(io_q.ctrMaxSz, io_q.sz);
		pthread_cond_signal(&io_q.wakeup_worker);
	}
	pthread_mutex_unlock(&io_q.mut);

	if (dispatchInline == 1) {
		free(n);
		processWorkItem(epd);
	}
finalize_it:
	if (iRet != RS_RET_OK) {
		if (n == NULL) {
			errmsg.LogError(0, iRet, "imptcp: couldn't allocate memory to enqueue io-request - ignored");
		}
	}
	RETiRet;
}