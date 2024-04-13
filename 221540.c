
static void io_clean_op(struct io_kiocb *req)
{
	if (req->flags & REQ_F_BUFFER_SELECTED) {
		switch (req->opcode) {
		case IORING_OP_READV:
		case IORING_OP_READ_FIXED:
		case IORING_OP_READ:
			kfree((void *)(unsigned long)req->rw.addr);
			break;
		case IORING_OP_RECVMSG:
		case IORING_OP_RECV:
			kfree(req->sr_msg.kbuf);
			break;
		}
		req->flags &= ~REQ_F_BUFFER_SELECTED;
	}

	if (req->flags & REQ_F_NEED_CLEANUP) {
		switch (req->opcode) {
		case IORING_OP_READV:
		case IORING_OP_READ_FIXED:
		case IORING_OP_READ:
		case IORING_OP_WRITEV:
		case IORING_OP_WRITE_FIXED:
		case IORING_OP_WRITE: {
			struct io_async_rw *io = req->async_data;
			if (io->free_iovec)
				kfree(io->free_iovec);
			break;
			}
		case IORING_OP_RECVMSG:
		case IORING_OP_SENDMSG: {
			struct io_async_msghdr *io = req->async_data;

			kfree(io->free_iov);
			break;
			}
		case IORING_OP_SPLICE:
		case IORING_OP_TEE:
			if (!(req->splice.flags & SPLICE_F_FD_IN_FIXED))
				io_put_file(req->splice.file_in);
			break;
		case IORING_OP_OPENAT:
		case IORING_OP_OPENAT2:
			if (req->open.filename)
				putname(req->open.filename);
			break;
		case IORING_OP_RENAMEAT:
			putname(req->rename.oldpath);
			putname(req->rename.newpath);
			break;
		case IORING_OP_UNLINKAT:
			putname(req->unlink.filename);
			break;
		}
		req->flags &= ~REQ_F_NEED_CLEANUP;
	}
	if ((req->flags & REQ_F_POLLED) && req->apoll) {
		kfree(req->apoll->double_poll);
		kfree(req->apoll);
		req->apoll = NULL;
	}
	if (req->flags & REQ_F_INFLIGHT) {
		struct io_uring_task *tctx = req->task->io_uring;

		atomic_dec(&tctx->inflight_tracked);
		req->flags &= ~REQ_F_INFLIGHT;
	}