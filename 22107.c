getPeerNames(prop_t **peerName, prop_t **peerIP, struct sockaddr *pAddr, sbool bUXServer)
{
	int error;
	uchar szIP[NI_MAXHOST] = "";
	uchar szHname[NI_MAXHOST] = "";
	struct addrinfo hints, *res;
	sbool bMaliciousHName = 0;
	
	DEFiRet;

	*peerName = NULL;
	*peerIP = NULL;

	if (bUXServer) {
		strcpy((char *) szHname, (char *) glbl.GetLocalHostName());
		strcpy((char *) szIP, (char *) glbl.GetLocalHostIP());
	} else {
		error = getnameinfo(pAddr, SALEN(pAddr), (char *) szIP, sizeof(szIP), NULL, 0, NI_NUMERICHOST);
		if (error) {
			DBGPRINTF("Malformed from address %s\n", gai_strerror(error));
			strcpy((char *) szHname, "???");
			strcpy((char *) szIP, "???");
			ABORT_FINALIZE(RS_RET_INVALID_HNAME);
		}

		if (!glbl.GetDisableDNS()) {
			error = getnameinfo(pAddr, SALEN(pAddr), (char *) szHname, NI_MAXHOST, NULL, 0, NI_NAMEREQD);
			if (error == 0) {
				memset(&hints, 0, sizeof(struct addrinfo));
				hints.ai_flags = AI_NUMERICHOST;
				hints.ai_socktype = SOCK_STREAM;
				/* we now do a lookup once again. This one should fail,
				 * because we should not have obtained a non-numeric address. If
				 * we got a numeric one, someone messed with DNS!
				 */
				if (getaddrinfo((char *) szHname, NULL, &hints, &res) == 0) {
					freeaddrinfo(res);
					/* OK, we know we have evil, so let's indicate this to our caller */
					snprintf((char *) szHname, NI_MAXHOST, "[MALICIOUS:IP=%s]", szIP);
					DBGPRINTF("Malicious PTR record, IP = \"%s\" HOST = \"%s\"", szIP, szHname);
					bMaliciousHName = 1;
				}
			} else {
				strcpy((char *) szHname, (char *) szIP);
			}
		} else {
			strcpy((char *) szHname, (char *) szIP);
		}
	}

	/* We now have the names, so now let's allocate memory and store them permanently. */
	CHKiRet(prop.Construct(peerName));
	CHKiRet(prop.SetString(*peerName, szHname, ustrlen(szHname)));
	CHKiRet(prop.ConstructFinalize(*peerName));
	CHKiRet(prop.Construct(peerIP));
	CHKiRet(prop.SetString(*peerIP, szIP, ustrlen(szIP)));
	CHKiRet(prop.ConstructFinalize(*peerIP));

finalize_it:
	if(iRet != RS_RET_OK) {
		if(*peerName != NULL)
			prop.Destruct(peerName);
		if(*peerIP != NULL)
			prop.Destruct(peerIP);
	}
	if(bMaliciousHName)
		iRet = RS_RET_MALICIOUS_HNAME;
	RETiRet;
}