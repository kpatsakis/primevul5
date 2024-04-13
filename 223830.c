srs_set_separator(srs_t *srs, char value)
{
	if (strchr(srs_separators, value) == NULL)
		return SRS_ESEPARATORINVALID;
	srs->separator = value;
	return SRS_SUCCESS;
}