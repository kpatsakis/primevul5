sessActivity(ptcpsess_t *pSess, int *continue_polling)
{
	int lenRcv;
	int lenBuf;
	uchar *peerName;
	int lenPeer;
	int remsock = 0; /* init just to keep compiler happy... :-( */
	sbool bEmitOnClose = 0;
	char rcvBuf[128*1024];
	DEFiRet;

	DBGPRINTF("imptcp: new activity on session socket %d\n", pSess->sock);

	while(1) {
		lenBuf = sizeof(rcvBuf);
		lenRcv = recv(pSess->sock, rcvBuf, lenBuf, 0);

		if(lenRcv > 0) {
			/* have data, process it */
			DBGPRINTF("imptcp: data(%d) on socket %d: %s\n", lenBuf, pSess->sock, rcvBuf);
			CHKiRet(DataRcvd(pSess, rcvBuf, lenRcv));
		} else if (lenRcv == 0) {
			/* session was closed, do clean-up */
			if(pSess->pLstn->pSrv->bEmitMsgOnClose) {
				prop.GetString(pSess->peerName, &peerName, &lenPeer),
				remsock = pSess->sock;
				bEmitOnClose = 1;
			}
			*continue_polling = 0;
			CHKiRet(closeSess(pSess)); /* close may emit more messages in strmzip mode! */
			if(bEmitOnClose) {
				errmsg.LogError(0, RS_RET_PEER_CLOSED_CONN, "imptcp session %d closed by "
					  	"remote peer %s.", remsock, peerName);
			}
			break;
		} else {
			if(errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			DBGPRINTF("imptcp: error on session socket %d - closed.\n", pSess->sock);
			*continue_polling = 0;
			closeSess(pSess); /* try clean-up by dropping session */
			break;
		}
	}

finalize_it:
	RETiRet;
}