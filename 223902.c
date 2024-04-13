static inline void queueDrain(qqueue_t *pThis)
{
	void *pUsr;
	ASSERT(pThis != NULL);

	BEGINfunc
	DBGOPRINT((obj_t*) pThis, "queue (type %d) will lose %d messages, destroying...\n", pThis->qType, pThis->iQueueSize);
	/* iQueueSize is not decremented by qDel(), so we need to do it ourselves */
	while(ATOMIC_DEC_AND_FETCH(&pThis->iQueueSize, &pThis->mutQueueSize) > 0) {
		pThis->qDeq(pThis, &pUsr);
		if(pUsr != NULL) {
			objDestruct(pUsr);
		}
		pThis->qDel(pThis);
	}
	ENDfunc
}