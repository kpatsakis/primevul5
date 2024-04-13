stopWorkerPool(void)
{
	int i;
	DBGPRINTF("imptcp: stoping worker pool\n");
	pthread_mutex_lock(&io_q.mut);
	pthread_cond_broadcast(&io_q.wakeup_worker); /* awake wrkr if not running */
	pthread_mutex_unlock(&io_q.mut);
	for(i = 0 ; i < runModConf->wrkrMax ; ++i) {
		pthread_join(wrkrInfo[i].tid, NULL);
		DBGPRINTF("imptcp: info: worker %d was called %llu times\n", i, wrkrInfo[i].numCalled);
	}
    free(wrkrInfo);
}