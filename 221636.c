static inline void io_put_req_deferred(struct io_kiocb *req, int refs)
{
	if (req_ref_sub_and_test(req, refs))
		io_free_req_deferred(req);
}