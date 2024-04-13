static void cil_reset_validatetrans(struct cil_validatetrans *vt)
{
	cil_list_destroy(&vt->datum_expr, CIL_FALSE);
}