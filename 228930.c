filter_data(uint64_t reqid, const char *line)
{
	struct filter_session  *fs;

	fs = tree_xget(&sessions, reqid);

	filter_data_internal(fs, 0, reqid, line);
}