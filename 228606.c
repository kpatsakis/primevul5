static void cil_reset_sidcontext(struct cil_sidcontext *sidcontext)
{
	if (sidcontext->context_str == NULL) {
		cil_reset_context(sidcontext->context);
	}
}