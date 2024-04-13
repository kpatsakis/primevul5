static inline void req_ref_put(struct io_kiocb *req)
{
	WARN_ON_ONCE(req_ref_put_and_test(req));
}