static void cil_reset_constrain(struct cil_constrain *con)
{
	cil_reset_classperms_list(con->classperms);
	cil_list_destroy(&con->datum_expr, CIL_FALSE);
}