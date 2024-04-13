static void cil_reset_typeattributeset(struct cil_typeattributeset *tas)
{
	cil_list_destroy(&tas->datum_expr, CIL_FALSE);
}