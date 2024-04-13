static int io_req_defer_prep(struct io_kiocb *req,
			     const struct io_uring_sqe *sqe)
{
	ssize_t ret = 0;

	if (!sqe)
		return 0;

	if (io_alloc_async_ctx(req))
		return -EAGAIN;
	ret = io_prep_work_files(req);
	if (unlikely(ret))
		return ret;

	switch (req->opcode) {
	case IORING_OP_NOP:
		break;
	case IORING_OP_READV:
	case IORING_OP_READ_FIXED:
	case IORING_OP_READ:
		ret = io_read_prep(req, sqe, true);
		break;
	case IORING_OP_WRITEV:
	case IORING_OP_WRITE_FIXED:
	case IORING_OP_WRITE:
		ret = io_write_prep(req, sqe, true);
		break;
	case IORING_OP_POLL_ADD:
		ret = io_poll_add_prep(req, sqe);
		break;
	case IORING_OP_POLL_REMOVE:
		ret = io_poll_remove_prep(req, sqe);
		break;
	case IORING_OP_FSYNC:
		ret = io_prep_fsync(req, sqe);
		break;
	case IORING_OP_SYNC_FILE_RANGE:
		ret = io_prep_sfr(req, sqe);
		break;
	case IORING_OP_SENDMSG:
	case IORING_OP_SEND:
		ret = io_sendmsg_prep(req, sqe);
		break;
	case IORING_OP_RECVMSG:
	case IORING_OP_RECV:
		ret = io_recvmsg_prep(req, sqe);
		break;
	case IORING_OP_CONNECT:
		ret = io_connect_prep(req, sqe);
		break;
	case IORING_OP_TIMEOUT:
		ret = io_timeout_prep(req, sqe, false);
		break;
	case IORING_OP_TIMEOUT_REMOVE:
		ret = io_timeout_remove_prep(req, sqe);
		break;
	case IORING_OP_ASYNC_CANCEL:
		ret = io_async_cancel_prep(req, sqe);
		break;
	case IORING_OP_LINK_TIMEOUT:
		ret = io_timeout_prep(req, sqe, true);
		break;
	case IORING_OP_ACCEPT:
		ret = io_accept_prep(req, sqe);
		break;
	case IORING_OP_FALLOCATE:
		ret = io_fallocate_prep(req, sqe);
		break;
	case IORING_OP_OPENAT:
		ret = io_openat_prep(req, sqe);
		break;
	case IORING_OP_CLOSE:
		ret = io_close_prep(req, sqe);
		break;
	case IORING_OP_FILES_UPDATE:
		ret = io_files_update_prep(req, sqe);
		break;
	case IORING_OP_STATX:
		ret = io_statx_prep(req, sqe);
		break;
	case IORING_OP_FADVISE:
		ret = io_fadvise_prep(req, sqe);
		break;
	case IORING_OP_MADVISE:
		ret = io_madvise_prep(req, sqe);
		break;
	case IORING_OP_OPENAT2:
		ret = io_openat2_prep(req, sqe);
		break;
	case IORING_OP_EPOLL_CTL:
		ret = io_epoll_ctl_prep(req, sqe);
		break;
	case IORING_OP_SPLICE:
		ret = io_splice_prep(req, sqe);
		break;
	case IORING_OP_PROVIDE_BUFFERS:
		ret = io_provide_buffers_prep(req, sqe);
		break;
	case IORING_OP_REMOVE_BUFFERS:
		ret = io_remove_buffers_prep(req, sqe);
		break;
	case IORING_OP_TEE:
		ret = io_tee_prep(req, sqe);
		break;
	default:
		printk_once(KERN_WARNING "io_uring: unhandled opcode %d\n",
				req->opcode);
		ret = -EINVAL;
		break;
	}

	return ret;
}