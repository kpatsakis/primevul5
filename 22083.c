	for(inst = pModConf->root ; inst != NULL ; ) {
		free(inst->pszBindPort);
		free(inst->pszBindPath);
		free(inst->pszBindAddr);
		free(inst->pszBindRuleset);
		free(inst->pszInputName);
		free(inst->dfltTZ);
		del = inst;
		inst = inst->next;
		free(del);
	}