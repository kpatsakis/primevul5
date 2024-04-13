
static int io_req_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	switch (req->opcode) {
	case IORING_OP_NOP:
		return 0;
	case IORING_OP_READV:
	case IORING_OP_READ_FIXED:
	case IORING_OP_READ:
		return io_read_prep(req, sqe);
	case IORING_OP_WRITEV:
	case IORING_OP_WRITE_FIXED:
	case IORING_OP_WRITE:
		return io_write_prep(req, sqe);
	case IORING_OP_POLL_ADD:
		return io_poll_add_prep(req, sqe);
	case IORING_OP_POLL_REMOVE:
		return io_poll_update_prep(req, sqe);
	case IORING_OP_FSYNC:
		return io_fsync_prep(req, sqe);
	case IORING_OP_SYNC_FILE_RANGE:
		return io_sfr_prep(req, sqe);
	case IORING_OP_SENDMSG:
	case IORING_OP_SEND:
		return io_sendmsg_prep(req, sqe);
	case IORING_OP_RECVMSG:
	case IORING_OP_RECV:
		return io_recvmsg_prep(req, sqe);
	case IORING_OP_CONNECT:
		return io_connect_prep(req, sqe);
	case IORING_OP_TIMEOUT:
		return io_timeout_prep(req, sqe, false);
	case IORING_OP_TIMEOUT_REMOVE:
		return io_timeout_remove_prep(req, sqe);
	case IORING_OP_ASYNC_CANCEL:
		return io_async_cancel_prep(req, sqe);
	case IORING_OP_LINK_TIMEOUT:
		return io_timeout_prep(req, sqe, true);
	case IORING_OP_ACCEPT:
		return io_accept_prep(req, sqe);
	case IORING_OP_FALLOCATE:
		return io_fallocate_prep(req, sqe);
	case IORING_OP_OPENAT:
		return io_openat_prep(req, sqe);
	case IORING_OP_CLOSE:
		return io_close_prep(req, sqe);
	case IORING_OP_FILES_UPDATE:
		return io_rsrc_update_prep(req, sqe);
	case IORING_OP_STATX:
		return io_statx_prep(req, sqe);
	case IORING_OP_FADVISE:
		return io_fadvise_prep(req, sqe);
	case IORING_OP_MADVISE:
		return io_madvise_prep(req, sqe);
	case IORING_OP_OPENAT2:
		return io_openat2_prep(req, sqe);
	case IORING_OP_EPOLL_CTL:
		return io_epoll_ctl_prep(req, sqe);
	case IORING_OP_SPLICE:
		return io_splice_prep(req, sqe);
	case IORING_OP_PROVIDE_BUFFERS:
		return io_provide_buffers_prep(req, sqe);
	case IORING_OP_REMOVE_BUFFERS:
		return io_remove_buffers_prep(req, sqe);
	case IORING_OP_TEE:
		return io_tee_prep(req, sqe);
	case IORING_OP_SHUTDOWN:
		return io_shutdown_prep(req, sqe);
	case IORING_OP_RENAMEAT:
		return io_renameat_prep(req, sqe);
	case IORING_OP_UNLINKAT:
		return io_unlinkat_prep(req, sqe);
	}

	printk_once(KERN_WARNING "io_uring: unhandled opcode %d\n",
			req->opcode);
	return -EINVAL;