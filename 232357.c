static bool io_poll_remove_one(struct io_kiocb *req)
{
	bool do_complete;

	if (req->opcode == IORING_OP_POLL_ADD) {
		io_poll_remove_double(req, req->io);
		do_complete = __io_poll_remove_one(req, &req->poll);
	} else {
		struct async_poll *apoll = req->apoll;

		io_poll_remove_double(req, apoll->double_poll);

		/* non-poll requests have submit ref still */
		do_complete = __io_poll_remove_one(req, &apoll->poll);
		if (do_complete) {
			io_put_req(req);
			kfree(apoll->double_poll);
			kfree(apoll);
		}
	}

	if (do_complete) {
		io_cqring_fill_event(req, -ECANCELED);
		io_commit_cqring(req->ctx);
		req->flags |= REQ_F_COMP_LOCKED;
		io_put_req(req);
	}

	return do_complete;
}