static inline void req_ref_get(struct io_kiocb *req)
{
	WARN_ON_ONCE(req_ref_zero_or_close_to_overflow(req));
	atomic_inc(&req->refs);
}