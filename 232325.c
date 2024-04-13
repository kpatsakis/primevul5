static void __io_clean_op(struct io_kiocb *req)
{
	struct io_async_ctx *io = req->io;

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
		case IORING_OP_WRITE:
			if (io->rw.iov != io->rw.fast_iov)
				kfree(io->rw.iov);
			break;
		case IORING_OP_RECVMSG:
		case IORING_OP_SENDMSG:
			if (io->msg.iov != io->msg.fast_iov)
				kfree(io->msg.iov);
			break;
		case IORING_OP_SPLICE:
		case IORING_OP_TEE:
			io_put_file(req, req->splice.file_in,
				    (req->splice.flags & SPLICE_F_FD_IN_FIXED));
			break;
		}
		req->flags &= ~REQ_F_NEED_CLEANUP;
	}
}