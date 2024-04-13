DequeueForConsumer(qqueue_t *pThis, wti_t *pWti)
{
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	ISOBJ_TYPE_assert(pWti, wti);

	CHKiRet(DequeueConsumable(pThis, pWti));

	if(pWti->batch.nElem == 0)
		ABORT_FINALIZE(RS_RET_IDLE);


finalize_it:
	RETiRet;
}