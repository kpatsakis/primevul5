static inline bool req_ref_put_and_test(struct io_kiocb *req)
{
	WARN_ON_ONCE(req_ref_zero_or_close_to_overflow(req));
	return atomic_dec_and_test(&req->refs);
}