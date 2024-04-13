static void cil_reset_role(struct cil_role *role)
{
	/* reset the bounds to NULL during a re-resolve */
	role->bounds = NULL;
}