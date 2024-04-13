qqueueMultiEnqObjDirect(qqueue_t *pThis, multi_submit_t *pMultiSub)
{
	int i;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	assert(pMultiSub != NULL);

	for(i = 0 ; i < pMultiSub->nElem ; ++i) {
		CHKiRet(qAddDirect(pThis, (void*)pMultiSub->ppMsgs[i]));
	}

finalize_it:
	RETiRet;
}