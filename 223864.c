static rsRetVal qAddLinkedList(qqueue_t *pThis, void* pUsr)
{
	qLinkedList_t *pEntry;
	DEFiRet;

	CHKmalloc((pEntry = (qLinkedList_t*) MALLOC(sizeof(qLinkedList_t))));

	pEntry->pNext = NULL;
	pEntry->pUsr = pUsr;

	if(pThis->tVars.linklist.pDelRoot == NULL) {
		pThis->tVars.linklist.pDelRoot = pThis->tVars.linklist.pDeqRoot = pThis->tVars.linklist.pLast = pEntry;
	} else {
		pThis->tVars.linklist.pLast->pNext = pEntry;
		pThis->tVars.linklist.pLast = pEntry;
	}

	if(pThis->tVars.linklist.pDeqRoot == NULL) {
		pThis->tVars.linklist.pDeqRoot = pEntry;
	}

finalize_it:
	RETiRet;
}