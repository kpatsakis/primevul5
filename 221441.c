static inline bool req_ref_inc_not_zero(struct io_kiocb *req)
{
	return atomic_inc_not_zero(&req->refs);
}