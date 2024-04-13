	if(epollfd < 0) {
		errmsg.LogError(0, RS_RET_EPOLL_CR_FAILED, "error: epoll_create() failed");
		ABORT_FINALIZE(RS_RET_NO_RUN);
	}