static inline struct io_kiocb *io_put_req_find_next(struct io_kiocb *req)
{
	struct io_kiocb *nxt = NULL;

	if (req_ref_put_and_test(req)) {
		nxt = io_req_find_next(req);
		__io_free_req(req);
	}
	return nxt;
}