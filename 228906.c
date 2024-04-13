lka_filter_data_begin(uint64_t reqid)
{
	struct filter_session  *fs;
	int	sp[2];
	int	fd = -1;

	fs = tree_xget(&sessions, reqid);

	if (socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, sp) == -1)
		goto end;
	io_set_nonblocking(sp[0]);
	io_set_nonblocking(sp[1]);
	fd = sp[0];
	fs->io = io_new();
	io_set_fd(fs->io, sp[1]);
	io_set_callback(fs->io, filter_session_io, fs);

end:
	m_create(p_pony, IMSG_FILTER_SMTP_DATA_BEGIN, 0, 0, fd);
	m_add_id(p_pony, reqid);
	m_add_int(p_pony, fd != -1 ? 1 : 0);
	m_close(p_pony);
	log_trace(TRACE_FILTERS, "%016"PRIx64" filters data-begin fd=%d", reqid, fd);
}