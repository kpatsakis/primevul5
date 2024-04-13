evbuffer_readfile(struct evbuffer *buf, evutil_socket_t fd, ev_ssize_t howmuch)
{
	int result;
	int nchains, n;
	struct evbuffer_iovec v[2];

	EVBUFFER_LOCK(buf);

	if (buf->freeze_end) {
		result = -1;
		goto done;
	}

	if (howmuch < 0)
		howmuch = 16384;


	/* XXX we _will_ waste some space here if there is any space left
	 * over on buf->last. */
	nchains = evbuffer_reserve_space(buf, howmuch, v, 2);
	if (nchains < 1 || nchains > 2) {
		result = -1;
		goto done;
	}
	n = read((int)fd, v[0].iov_base, (unsigned int)v[0].iov_len);
	if (n <= 0) {
		result = n;
		goto done;
	}
	v[0].iov_len = (IOV_LEN_TYPE) n; /* XXXX another problem with big n.*/
	if (nchains > 1) {
		n = read((int)fd, v[1].iov_base, (unsigned int)v[1].iov_len);
		if (n <= 0) {
			result = (unsigned long) v[0].iov_len;
			evbuffer_commit_space(buf, v, 1);
			goto done;
		}
		v[1].iov_len = n;
	}
	evbuffer_commit_space(buf, v, nchains);

	result = n;
done:
	EVBUFFER_UNLOCK(buf);
	return result;
}