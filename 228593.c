static void cil_reset_booleanif(struct cil_booleanif *bif)
{
	cil_list_destroy(&bif->datum_expr, CIL_FALSE);
}