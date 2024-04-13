static inline void io_put_req(struct io_kiocb *req)
{
	if (req_ref_put_and_test(req))
		io_free_req(req);
}