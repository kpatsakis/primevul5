qqueueMultiEnqObjNonDirect(qqueue_t *pThis, multi_submit_t *pMultiSub)
{
	int iCancelStateSave;
	int i;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	assert(pMultiSub != NULL);

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &iCancelStateSave);
	d_pthread_mutex_lock(pThis->mut);
	for(i = 0 ; i < pMultiSub->nElem ; ++i) {
		CHKiRet(doEnqSingleObj(pThis, pMultiSub->ppMsgs[i]->flowCtlType, (void*)pMultiSub->ppMsgs[i]));
	}
	qqueueChkPersist(pThis, pMultiSub->nElem);

finalize_it:
	/* make sure at least one worker is running. */
	qqueueAdviseMaxWorkers(pThis);
	/* and release the mutex */
	d_pthread_mutex_unlock(pThis->mut);
	pthread_setcancelstate(iCancelStateSave, NULL);
	DBGOPRINT((obj_t*) pThis, "MultiEnqObj advised worker start\n");

	RETiRet;
}