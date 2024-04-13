	for(i = 0 ; i < modpblk.nParams ; ++i) {
		if(!pvals[i].bUsed)
			continue;
		if(!strcmp(modpblk.descr[i].name, "threads")) {
			loadModConf->wrkrMax = (int) pvals[i].val.d.n;
		} else if(!strcmp(modpblk.descr[i].name, "processOnPoller")) {
			loadModConf->bProcessOnPoller = (int) pvals[i].val.d.n;
		} else {
			dbgprintf("imptcp: program error, non-handled "
			  "param '%s' in beginCnfLoad\n", modpblk.descr[i].name);
		}
	}