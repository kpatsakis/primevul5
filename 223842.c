	if(pThis->bIsDA && getPhysicalQueueSize(pThis) > 0 && pThis->bSaveOnShutdown) {
		CHKiRet(DoSaveOnShutdown(pThis));
	}