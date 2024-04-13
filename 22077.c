std_checkRuleset_genErrMsg(__attribute__((unused)) modConfData_t *modConf, instanceConf_t *inst)
{
	errmsg.LogError(0, NO_ERRCODE, "imptcp: ruleset '%s' for port %s not found - "
			"using default ruleset instead", inst->pszBindRuleset,
			inst->pszBindPort);
}