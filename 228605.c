static inline void cil_reset_cats(struct cil_cats *cats)
{
	if (cats != NULL) {
		cats->evaluated = CIL_FALSE;
		cil_list_destroy(&cats->datum_expr, CIL_FALSE);
	}
}