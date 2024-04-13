static void io_complete_rw_iopoll(struct kiocb *kiocb, long res, long res2)
{
	struct io_kiocb *req = container_of(kiocb, struct io_kiocb, rw.kiocb);

	if (kiocb->ki_flags & IOCB_WRITE)
		kiocb_end_write(req);
	if (unlikely(res != req->result)) {
		if (!(res == -EAGAIN && io_rw_should_reissue(req) &&
		    io_resubmit_prep(req))) {
			req_set_fail_links(req);
			req->flags |= REQ_F_DONT_REISSUE;
		}
	}

	WRITE_ONCE(req->result, res);
	/* order with io_iopoll_complete() checking ->result */
	smp_wmb();
	WRITE_ONCE(req->iopoll_completed, 1);
}