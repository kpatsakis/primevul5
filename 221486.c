static inline void req_set_fail_links(struct io_kiocb *req)
{
	if (req->flags & REQ_F_LINK)
		req->flags |= REQ_F_FAIL_LINK;
}