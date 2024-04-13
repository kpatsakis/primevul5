	__must_hold(&req->ctx->completion_lock)
{
	bool posted = false;

	if (likely(req->flags & REQ_F_LINK_TIMEOUT))
		posted = io_kill_linked_timeout(req);
	if (unlikely((req->flags & REQ_F_FAIL_LINK) &&
		     !(req->flags & REQ_F_HARDLINK))) {
		posted |= (req->link != NULL);
		io_fail_links(req);
	}
	return posted;
}