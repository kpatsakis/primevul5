EnableKeepAlive(ptcplstn_t *pLstn, int sock)
{
	int ret;
	int optval;
	socklen_t optlen;
	DEFiRet;

	optval = 1;
	optlen = sizeof(optval);
	ret = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
	if(ret < 0) {
		dbgprintf("EnableKeepAlive socket call returns error %d\n", ret);
		ABORT_FINALIZE(RS_RET_ERR);
	}

#	if defined(TCP_KEEPCNT)
	if(pLstn->pSrv->iKeepAliveProbes > 0) {
		optval = pLstn->pSrv->iKeepAliveProbes;
		optlen = sizeof(optval);
		ret = setsockopt(sock, SOL_TCP, TCP_KEEPCNT, &optval, optlen);
	} else {
		ret = 0;
	}
#	else
	ret = -1;
#	endif
	if(ret < 0) {
		errmsg.LogError(ret, NO_ERRCODE, "imptcp cannot set keepalive probes - ignored");
	}

#	if defined(TCP_KEEPCNT)
	if(pLstn->pSrv->iKeepAliveTime > 0) {
		optval = pLstn->pSrv->iKeepAliveTime;
		optlen = sizeof(optval);
		ret = setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, &optval, optlen);
	} else {
		ret = 0;
	}
#	else
	ret = -1;
#	endif
	if(ret < 0) {
		errmsg.LogError(ret, NO_ERRCODE, "imptcp cannot set keepalive time - ignored");
	}

#	if defined(TCP_KEEPCNT)
	if(pLstn->pSrv->iKeepAliveIntvl > 0) {
		optval = pLstn->pSrv->iKeepAliveIntvl;
		optlen = sizeof(optval);
		ret = setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, &optval, optlen);
	} else {
		ret = 0;
	}
#	else
	ret = -1;
#	endif
	if(ret < 0) {
		errmsg.LogError(errno, NO_ERRCODE, "imptcp cannot set keepalive intvl - ignored");
	}

	dbgprintf("KEEPALIVE enabled for socket %d\n", sock);

finalize_it:
	RETiRet;
}