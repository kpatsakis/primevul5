static bool req_need_defer(struct io_kiocb *req, u32 seq)
{
	if (unlikely(req->flags & REQ_F_IO_DRAIN)) {
		struct io_ring_ctx *ctx = req->ctx;

		return seq + ctx->cq_extra != ctx->cached_cq_tail
				+ READ_ONCE(ctx->cached_cq_overflow);
	}

	return false;
}