static rsRetVal qConstructLinkedList(qqueue_t *pThis)
{
	DEFiRet;

	ASSERT(pThis != NULL);

	pThis->tVars.linklist.pDeqRoot = NULL;
	pThis->tVars.linklist.pDelRoot = NULL;
	pThis->tVars.linklist.pLast = NULL;

	qqueueChkIsDA(pThis);

	RETiRet;
}