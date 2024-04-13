	if(pThis->qType != QUEUETYPE_DIRECT && pThis->pWtpReg != NULL) {
		wtpDestruct(&pThis->pWtpReg);
	}