getLogicalQueueSize(qqueue_t *pThis)
{
	return pThis->iQueueSize - pThis->nLogDeq;
}