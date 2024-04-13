static void cil_reset_default(struct cil_default *def)
{
	cil_list_destroy(&def->class_datums, CIL_FALSE);
}