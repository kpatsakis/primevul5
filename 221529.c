static int io_import_fixed(struct io_kiocb *req, int rw, struct iov_iter *iter)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_mapped_ubuf *imu = req->imu;
	u16 index, buf_index = req->buf_index;

	if (likely(!imu)) {
		if (unlikely(buf_index >= ctx->nr_user_bufs))
			return -EFAULT;
		index = array_index_nospec(buf_index, ctx->nr_user_bufs);
		imu = READ_ONCE(ctx->user_bufs[index]);
		req->imu = imu;
	}
	return __io_import_fixed(req, rw, iter, imu);
}