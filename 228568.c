static void cil_reset_type(struct cil_type *type)
{
	/* reset the bounds to NULL during a re-resolve */
	type->bounds = NULL;
}