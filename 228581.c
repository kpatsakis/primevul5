static void cil_reset_roleattributeset(struct cil_roleattributeset *ras)
{
	cil_list_destroy(&ras->datum_expr, CIL_FALSE);
}