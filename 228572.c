static void cil_reset_sid(struct cil_sid *sid)
{
	/* reset the context to NULL during a re-resolve */
	sid->context = NULL;
	sid->ordered = CIL_FALSE;
}