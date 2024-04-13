jpc_ms_t *jpc_ms_create(int type)
{
	jpc_ms_t *ms;
	const jpc_mstabent_t *mstabent;

	if (!(ms = jas_malloc(sizeof(jpc_ms_t)))) {
		return 0;
	}
	ms->id = type;
	ms->len = 0;
	mstabent = jpc_mstab_lookup(ms->id);
	ms->ops = &mstabent->ops;
	memset(&ms->parms, 0, sizeof(jpc_msparms_t));
	return ms;
}