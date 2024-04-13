	CHKiRet_Hdlr(qqueuePersist(pThis, QUEUE_NO_CHECKPOINT)) {
		DBGOPRINT((obj_t*) pThis, "error %d persisting queue - data lost!\n", iRet);
	}