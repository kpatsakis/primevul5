static void cil_reset_user(struct cil_user *user)
{
	/* reset the bounds to NULL during a re-resolve */
	user->bounds = NULL;
	user->dftlevel = NULL;
	user->range = NULL;
}