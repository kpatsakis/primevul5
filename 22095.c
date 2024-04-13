startWorkerPool(void)
{
	int i;
	wrkrRunning = 0;
	DBGPRINTF("imptcp: starting worker pool, %d workers\n", runModConf->wrkrMax);
    wrkrInfo = calloc(runModConf->wrkrMax, sizeof(struct wrkrInfo_s));
    if (wrkrInfo == NULL) {
        DBGPRINTF("imptcp: worker-info array allocation failed.\n");
        return;
    }
	for(i = 0 ; i < runModConf->wrkrMax ; ++i) {
		/* init worker info structure! */
		wrkrInfo[i].numCalled = 0;
		pthread_create(&wrkrInfo[i].tid, &wrkrThrdAttr, wrkr, &(wrkrInfo[i]));
	}

}