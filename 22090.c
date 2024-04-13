addEPollSock(epolld_type_t typ, void *ptr, int sock, epolld_t **pEpd)
{
	DEFiRet;
	epolld_t *epd = NULL;

	CHKmalloc(epd = calloc(1, sizeof(epolld_t)));
	epd->typ = typ;
	epd->ptr = ptr;
	epd->sock = sock;
	*pEpd = epd;
	epd->ev.events = EPOLLIN|EPOLLET|EPOLLONESHOT;
	epd->ev.data.ptr = (void*) epd;

	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &(epd->ev)) != 0) {
		char errStr[1024];
		int eno = errno;
		errmsg.LogError(0, RS_RET_EPOLL_CTL_FAILED, "os error (%d) during epoll ADD: %s",
			        eno, rs_strerror_r(eno, errStr, sizeof(errStr)));
		ABORT_FINALIZE(RS_RET_EPOLL_CTL_FAILED);
	}

	DBGPRINTF("imptcp: added socket %d to epoll[%d] set\n", sock, epollfd);

finalize_it:
	if(iRet != RS_RET_OK) {
		if (epd != NULL) {
			errmsg.LogError(0, RS_RET_INTERNAL_ERROR, "error: could not initialize mutex for ptcp "
			"connection for socket: %d", sock);
		}
		free(epd);
	}
	RETiRet;
}