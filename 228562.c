static void cil_reset_defaultrange(struct cil_defaultrange *def)
{
	cil_list_destroy(&def->class_datums, CIL_FALSE);
}