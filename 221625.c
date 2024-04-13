static void io_dismantle_req(struct io_kiocb *req)
{
	unsigned int flags = req->flags;

	if (io_req_needs_clean(req))
		io_clean_op(req);
	if (!(flags & REQ_F_FIXED_FILE))
		io_put_file(req->file);
	if (req->fixed_rsrc_refs)
		percpu_ref_put(req->fixed_rsrc_refs);
	if (req->async_data)
		kfree(req->async_data);
	if (req->work.creds) {
		put_cred(req->work.creds);
		req->work.creds = NULL;
	}
}