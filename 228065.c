int mnt_optstr_remove_option_at(char **optstr, char *begin, char *end)
{
	size_t sz;

	if (!optstr || !begin || !end)
		return -EINVAL;

	if ((begin == *optstr || *(begin - 1) == ',') && *end == ',')
		end++;

	sz = strlen(end);

	memmove(begin, end, sz + 1);
	if (!*begin && (begin > *optstr) && *(begin - 1) == ',')
		*(begin - 1) = '\0';

	return 0;
}