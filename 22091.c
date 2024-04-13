	for(i = 0 ; i < inppblk.nParams ; ++i) {
		if(!pvals[i].bUsed)
			continue;
		if(!strcmp(inppblk.descr[i].name, "port")) {
			inst->pszBindPort = (uchar*)es_str2cstr(pvals[i].val.d.estr, NULL);
		} else if(!strcmp(inppblk.descr[i].name, "address")) {
			inst->pszBindAddr = (uchar*)es_str2cstr(pvals[i].val.d.estr, NULL);
		} else if(!strcmp(inppblk.descr[i].name, "path")) {
			inst->pszBindPath = (uchar*)es_str2cstr(pvals[i].val.d.estr, NULL);
		} else if(!strcmp(inppblk.descr[i].name, "unlink")) {
			inst->bUnlink = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "fileowner")) {
			inst->fileUID = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "fileownernum")) {
			inst->fileUID = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "filegroup")) {
			inst->fileGID = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "filegroupnum")) {
			inst->fileGID = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "filecreatemode")) {
			inst->fCreateMode = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "failonpermsfailure")) {
			inst->bFailOnPerms = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "name")) {
			inst->pszInputName = (uchar*)es_str2cstr(pvals[i].val.d.estr, NULL);
		} else if(!strcmp(inppblk.descr[i].name, "ruleset")) {
			inst->pszBindRuleset = (uchar*)es_str2cstr(pvals[i].val.d.estr, NULL);
		} else if(!strcmp(inppblk.descr[i].name, "supportoctetcountedframing")) {
			inst->bSuppOctetFram = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "framingfix.cisco.asa")) {
			inst->bSPFramingFix = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "compression.mode")) {
			cstr = es_str2cstr(pvals[i].val.d.estr, NULL);
			if(!strcasecmp(cstr, "stream:always")) {
				inst->compressionMode = COMPRESS_STREAM_ALWAYS;
			} else if(!strcasecmp(cstr, "none")) {
				inst->compressionMode = COMPRESS_NEVER;
			} else {
				errmsg.LogError(0, RS_RET_PARAM_ERROR, "omfwd: invalid value for 'compression.mode' "
					 "parameter (given is '%s')", cstr);
				free(cstr);
				ABORT_FINALIZE(RS_RET_PARAM_ERROR);
			}
			free(cstr);
		} else if(!strcmp(inppblk.descr[i].name, "keepalive")) {
			inst->bKeepAlive = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "keepalive.probes")) {
			inst->iKeepAliveProbes = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "keepalive.time")) {
			inst->iKeepAliveTime = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "keepalive.interval")) {
			inst->iKeepAliveIntvl = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "addtlframedelimiter")) {
			inst->iAddtlFrameDelim = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "notifyonconnectionclose")) {
			inst->bEmitMsgOnClose = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "defaulttz")) {
			inst->dfltTZ = (uchar*)es_str2cstr(pvals[i].val.d.estr, NULL);
		} else if(!strcmp(inppblk.descr[i].name, "ratelimit.burst")) {
			inst->ratelimitBurst = (int) pvals[i].val.d.n;
		} else if(!strcmp(inppblk.descr[i].name, "ratelimit.interval")) {
			inst->ratelimitInterval = (int) pvals[i].val.d.n;
		} else {
			dbgprintf("imptcp: program error, non-handled "
			  "param '%s'\n", inppblk.descr[i].name);
		}

		char *bindPort = (char *) inst->pszBindPort;
		char *bindPath = (char *) inst->pszBindPath;
		if ((bindPort == NULL || strlen(bindPort) < 1) && (bindPath == NULL || strlen (bindPath) < 1)) {
			errmsg.LogError(0, RS_RET_PARAM_ERROR, "imptcp: Must have either port or path defined");
			ABORT_FINALIZE(RS_RET_PARAM_ERROR);
		}
	}