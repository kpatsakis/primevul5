static void cil_reset_genfscon(struct cil_genfscon *genfscon)
{
	if (genfscon->context_str == NULL) {
		cil_reset_context(genfscon->context);
	}
}