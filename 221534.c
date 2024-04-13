static void io_req_complete_state(struct io_kiocb *req, long res,
				  unsigned int cflags)
{
	if (io_req_needs_clean(req))
		io_clean_op(req);
	req->result = res;
	req->compl.cflags = cflags;
	req->flags |= REQ_F_COMPLETE_INLINE;
}