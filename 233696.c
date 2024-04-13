acl_get_file_mode(const char *path_p)
{
	struct stat st;

	if (stat(path_p, &st) != 0)
		return NULL;
	return acl_from_mode(st.st_mode);
}