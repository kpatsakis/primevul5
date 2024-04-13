static void cil_reset_filecon(struct cil_filecon *filecon)
{
	if (filecon->context_str == NULL && filecon->context != NULL) {
		cil_reset_context(filecon->context);
	}
}