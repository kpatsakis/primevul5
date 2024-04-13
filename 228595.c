static void cil_reset_alias(struct cil_alias *alias)
{
	/* reset actual to NULL during a re-resolve */
	alias->actual = NULL;
}