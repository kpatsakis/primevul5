static inline bool io_req_needs_clean(struct io_kiocb *req)
{
	return req->flags & (REQ_F_BUFFER_SELECTED | REQ_F_NEED_CLEANUP |
				REQ_F_POLLED | REQ_F_INFLIGHT);
}