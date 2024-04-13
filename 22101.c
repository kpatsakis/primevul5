static rsRetVal startupUXSrv(ptcpsrv_t *pSrv) {
	DEFiRet;
	int sock;
	int sockflags;
	struct sockaddr_un local;

	uchar *path = pSrv->path == NULL ? UCHAR_CONSTANT("") : pSrv->path;
	DBGPRINTF("imptcp: creating listen unix socket at %s\n", path);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock < 0) {
		errmsg.LogError(errno, RS_RET_ERR_CRE_AFUX, "imptcp: error creating unix socket");
		ABORT_FINALIZE(RS_RET_ERR_CRE_AFUX);
	}

	local.sun_family = AF_UNIX;
	strncpy(local.sun_path, (char*) path, sizeof(local.sun_path));
	if (pSrv->bUnlink) {
		unlink(local.sun_path);
	}

	/* We use non-blocking IO! */
	if ((sockflags = fcntl(sock, F_GETFL)) != -1) {
		sockflags |= O_NONBLOCK;
		/* SETFL could fail too, so get it caught by the subsequent error check. */
		sockflags = fcntl(sock, F_SETFL, sockflags);
	}

	if (sockflags == -1) {
		errmsg.LogError(errno, RS_RET_ERR_CRE_AFUX, "imptcp: error setting fcntl(O_NONBLOCK) on unix socket");
		ABORT_FINALIZE(RS_RET_ERR_CRE_AFUX);
	}

	if (bind(sock, (struct sockaddr *)&local, SUN_LEN(&local)) < 0) {
		errmsg.LogError(errno, RS_RET_ERR_CRE_AFUX, "imptcp: error while binding unix socket");
		ABORT_FINALIZE(RS_RET_ERR_CRE_AFUX);
	}

	if (listen(sock, 5) < 0) {
		errmsg.LogError(errno, RS_RET_ERR_CRE_AFUX, "imptcp: unix socket listen error");
		ABORT_FINALIZE(RS_RET_ERR_CRE_AFUX);
	}

	if(chown(local.sun_path, pSrv->fileUID, pSrv->fileGID) != 0) {
		if(pSrv->bFailOnPerms) {
			errmsg.LogError(errno, RS_RET_ERR_CRE_AFUX, "imptcp: unix socket chown error");
			ABORT_FINALIZE(RS_RET_ERR_CRE_AFUX);
		}
	}

	if(chmod(local.sun_path, pSrv->fCreateMode) != 0) {
		if(pSrv->bFailOnPerms) {
			errmsg.LogError(errno, RS_RET_ERR_CRE_AFUX, "imptcp: unix socket chmod error");
			ABORT_FINALIZE(RS_RET_ERR_CRE_AFUX);
		}
	}

	CHKiRet(addLstn(pSrv, sock, 0));

finalize_it:
	if (iRet != RS_RET_OK) {
		if (sock != -1) {
			close(sock);
		}
	}

	RETiRet;
}