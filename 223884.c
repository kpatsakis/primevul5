static rsRetVal qDelLinkedList(qqueue_t *pThis)
{
	qLinkedList_t *pEntry;
	DEFiRet;

	pEntry = pThis->tVars.linklist.pDelRoot;

	if(pThis->tVars.linklist.pDelRoot == pThis->tVars.linklist.pLast) {
		pThis->tVars.linklist.pDelRoot = pThis->tVars.linklist.pDeqRoot = pThis->tVars.linklist.pLast = NULL;
	} else {
		pThis->tVars.linklist.pDelRoot = pEntry->pNext;
	}

	free(pEntry);

	RETiRet;
}