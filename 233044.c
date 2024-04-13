void jpc_ms_dump(jpc_ms_t *ms, FILE *out)
{
	const jpc_mstabent_t *mstabent;
	mstabent = jpc_mstab_lookup(ms->id);
	fprintf(out, "type = 0x%04"PRIxFAST16" (%s);", ms->id, mstabent->name);
	if (JPC_MS_HASPARMS(ms->id)) {
		fprintf(out, " len = %"PRIuFAST16";", ms->len + 2);
		if (ms->ops->dumpparms) {
			(*ms->ops->dumpparms)(ms, out);
		} else {
			fprintf(out, "\n");
		}
	} else {
		fprintf(out, "\n");
	}
}