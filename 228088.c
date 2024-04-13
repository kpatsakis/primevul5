static int set_uint_value(char **optstr, unsigned int num,
			char *begin, char *end, char **next)
{
	char buf[40];
	snprintf(buf, sizeof(buf), "%u", num);

	mnt_optstr_remove_option_at(optstr, begin, end);
	return insert_value(optstr, begin, buf, next);
}