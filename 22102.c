lstnActivity(ptcplstn_t *pLstn)
{
	int newSock = -1;
	prop_t *peerName;
	prop_t *peerIP;
	rsRetVal localRet;
	DEFiRet;

	DBGPRINTF("imptcp: new connection on listen socket %d\n", pLstn->sock);
	while(glbl.GetGlobalInputTermState() == 0) {
		localRet = AcceptConnReq(pLstn, &newSock, &peerName, &peerIP);
		if(localRet == RS_RET_NO_MORE_DATA || glbl.GetGlobalInputTermState() == 1) {
			break;
		}
		CHKiRet(localRet);
		localRet = addSess(pLstn, newSock, peerName, peerIP);
		if(localRet != RS_RET_OK) {
			close(newSock);
			prop.Destruct(&peerName);
			prop.Destruct(&peerIP);
			ABORT_FINALIZE(localRet);
		}
	}

finalize_it:
	RETiRet;
}