static int __class_reset_perm_values(__attribute__((unused)) hashtab_key_t k, hashtab_datum_t d, void *args)
{
	struct cil_perm *perm = (struct cil_perm *)d;

	perm->value -= *((int *)args);

	return SEPOL_OK;
}