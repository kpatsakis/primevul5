static void cil_reset_fsuse(struct cil_fsuse *fsuse)
{
	if (fsuse->context_str == NULL) {
		cil_reset_context(fsuse->context);
	}
}