static void cil_reset_ioportcon(struct cil_ioportcon *ioportcon)
{
	if (ioportcon->context_str == NULL) {
		cil_reset_context(ioportcon->context);
	}
}