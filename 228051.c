int mnt_optstr_next_option(char **optstr, char **name, size_t *namesz,
					char **value, size_t *valuesz)
{
	if (!optstr || !*optstr)
		return -EINVAL;
	return mnt_optstr_parse_next(optstr, name, namesz, value, valuesz);
}