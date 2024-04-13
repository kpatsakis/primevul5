static void cil_reset_ibendportcon(struct cil_ibendportcon *ibendportcon)
{
	if (!ibendportcon->context_str) {
		cil_reset_context(ibendportcon->context);
	}
}