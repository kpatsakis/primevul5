static rsRetVal qDeqLinkedList(qqueue_t *pThis, obj_t **ppUsr)
{
	qLinkedList_t *pEntry;
	DEFiRet;

	pEntry = pThis->tVars.linklist.pDeqRoot;
	ISOBJ_TYPE_assert(pEntry->pUsr, msg);
	*ppUsr = pEntry->pUsr;
	pThis->tVars.linklist.pDeqRoot = pEntry->pNext;

	RETiRet;
}