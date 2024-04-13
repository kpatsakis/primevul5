	if(pSrvRoot == NULL) {
		errmsg.LogError(0, RS_RET_NO_LSTN_DEFINED, "imptcp: no ptcp server defined, module can not run.");
		ABORT_FINALIZE(RS_RET_NO_RUN);
	}