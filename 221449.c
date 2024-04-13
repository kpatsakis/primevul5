static void io_req_complete_failed(struct io_kiocb *req, long res)
{
	req_set_fail_links(req);
	io_put_req(req);
	io_req_complete_post(req, res, 0);
}