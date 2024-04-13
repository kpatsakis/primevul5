static rsRetVal qqueueSetProperty(qqueue_t *pThis, var_t *pProp)
{
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	ASSERT(pProp != NULL);

 	if(isProp("iQueueSize")) {
		pThis->iQueueSize = pProp->val.num;
 	} else if(isProp("tVars.disk.sizeOnDisk")) {
		pThis->tVars.disk.sizeOnDisk = pProp->val.num;
 	} else if(isProp("tVars.disk.bytesRead")) {
		pThis->tVars.disk.bytesRead = pProp->val.num;
 	} else if(isProp("qType")) {
		if(pThis->qType != pProp->val.num)
			ABORT_FINALIZE(RS_RET_QTYPE_MISMATCH);
	}

finalize_it:
	RETiRet;
}