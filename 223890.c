qqueueAdd(qqueue_t *pThis, void *pUsr)
{
	DEFiRet;

	ASSERT(pThis != NULL);

	CHKiRet(pThis->qAdd(pThis, pUsr));

	if(pThis->qType != QUEUETYPE_DIRECT) {
		ATOMIC_INC(&pThis->iQueueSize, &pThis->mutQueueSize);
		DBGOPRINT((obj_t*) pThis, "entry added, size now log %d, phys %d entries\n",
			  getLogicalQueueSize(pThis), getPhysicalQueueSize(pThis));
	}

finalize_it:
	RETiRet;
}