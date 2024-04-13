static int io_recvmsg_prep_async(struct io_kiocb *req)
{
	int ret;

	ret = io_recvmsg_copy_hdr(req, req->async_data);
	if (!ret)
		req->flags |= REQ_F_NEED_CLEANUP;
	return ret;
}