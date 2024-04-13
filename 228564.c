static void cil_reset_sens(struct cil_sens *sens)
{
	/* during a re-resolve, we need to reset the categories associated with
	 * this sensitivity from a (sensitivitycategory) statement */
	cil_list_destroy(&sens->cats_list, CIL_FALSE);
	sens->ordered = CIL_FALSE;
}