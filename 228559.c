static void cil_reset_userattributeset(struct cil_userattributeset *uas)
{
	cil_list_destroy(&uas->datum_expr, CIL_FALSE);
}