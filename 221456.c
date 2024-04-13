
static struct file *io_file_get(struct io_submit_state *state,
				struct io_kiocb *req, int fd, bool fixed)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct file *file;

	if (fixed) {
		unsigned long file_ptr;

		if (unlikely((unsigned int)fd >= ctx->nr_user_files))
			return NULL;
		fd = array_index_nospec(fd, ctx->nr_user_files);
		file_ptr = io_fixed_file_slot(&ctx->file_table, fd)->file_ptr;
		file = (struct file *) (file_ptr & FFS_MASK);
		file_ptr &= ~FFS_MASK;
		/* mask in overlapping REQ_F and FFS bits */
		req->flags |= (file_ptr << REQ_F_ASYNC_READ_BIT);
		io_req_set_rsrc_node(req);
	} else {
		trace_io_uring_file_get(ctx, fd);
		file = __io_file_get(state, fd);

		/* we don't allow fixed io_uring files */
		if (file && unlikely(file->f_op == &io_uring_fops))
			io_req_track_inflight(req);
	}

	return file;