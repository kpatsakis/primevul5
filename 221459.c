static inline bool req_ref_sub_and_test(struct io_kiocb *req, int refs)
{
	WARN_ON_ONCE(req_ref_zero_or_close_to_overflow(req));
	return atomic_sub_and_test(refs, &req->refs);
}