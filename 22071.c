removeEPollSock(int sock, epolld_t *epd)
{
	DEFiRet;

	DBGPRINTF("imptcp: removing socket %d from epoll[%d] set\n", sock, epollfd);

	if(epoll_ctl(epollfd, EPOLL_CTL_DEL, sock, &(epd->ev)) != 0) {
		char errStr[1024];
		int eno = errno;
		errmsg.LogError(0, RS_RET_EPOLL_CTL_FAILED, "os error (%d) during epoll DEL: %s",
			        eno, rs_strerror_r(eno, errStr, sizeof(errStr)));
		ABORT_FINALIZE(RS_RET_EPOLL_CTL_FAILED);
	}

finalize_it:
	RETiRet;
}