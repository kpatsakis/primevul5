rsRetVal qqueueEnqObjDirectBatch(qqueue_t *pThis, batch_t *pBatch)
{
	DEFiRet;

	ASSERT(pThis != NULL);

	/* calling the consumer is quite different here than it is from a worker thread */
	/* we need to provide the consumer's return value back to the caller because in direct
	 * mode the consumer probably has a lot to convey (which get's lost in the other modes
	 * because they are asynchronous. But direct mode is deliberately synchronous.
	 * rgerhards, 2008-02-12
	 * We use our knowledge about the batch_t structure below, but without that, we
	 * pay a too-large performance toll... -- rgerhards, 2009-04-22
	 */
	iRet = pThis->pConsumer(pThis->pUsr, pBatch, &pThis->bShutdownImmediate);

	RETiRet;
}