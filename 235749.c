int update_variable_in_bank(struct secvar *update_var, const char *data,
			    const uint64_t dsize, struct list_head *bank)
{
	struct secvar *var;

	var = find_secvar(update_var->key, update_var->key_len, bank);
	if (!var)
		return OPAL_EMPTY;

        /* Reallocate the data memory, if there is change in data size */
	if (var->data_size < dsize)
		if (realloc_secvar(var, dsize))
			return OPAL_NO_MEM;

	if (dsize && data)
		memcpy(var->data, data, dsize);
	var->data_size = dsize;

        /* Clear the volatile bit only if updated with positive data size */
	if (dsize)
		var->flags &= ~SECVAR_FLAG_VOLATILE;
	else
		var->flags |= SECVAR_FLAG_VOLATILE;

	if (key_equals(update_var->key, "PK") || key_equals(update_var->key, "HWKH"))
		var->flags |= SECVAR_FLAG_PROTECTED;

	return 0;
}