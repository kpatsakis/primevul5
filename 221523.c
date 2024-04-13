
static int io_issue_sqe(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_ring_ctx *ctx = req->ctx;
	const struct cred *creds = NULL;
	int ret;

	if (req->work.creds && req->work.creds != current_cred())
		creds = override_creds(req->work.creds);

	switch (req->opcode) {
	case IORING_OP_NOP:
		ret = io_nop(req, issue_flags);
		break;
	case IORING_OP_READV:
	case IORING_OP_READ_FIXED:
	case IORING_OP_READ:
		ret = io_read(req, issue_flags);
		break;
	case IORING_OP_WRITEV:
	case IORING_OP_WRITE_FIXED:
	case IORING_OP_WRITE:
		ret = io_write(req, issue_flags);
		break;
	case IORING_OP_FSYNC:
		ret = io_fsync(req, issue_flags);
		break;
	case IORING_OP_POLL_ADD:
		ret = io_poll_add(req, issue_flags);
		break;
	case IORING_OP_POLL_REMOVE:
		ret = io_poll_update(req, issue_flags);
		break;
	case IORING_OP_SYNC_FILE_RANGE:
		ret = io_sync_file_range(req, issue_flags);
		break;
	case IORING_OP_SENDMSG:
		ret = io_sendmsg(req, issue_flags);
		break;
	case IORING_OP_SEND:
		ret = io_send(req, issue_flags);
		break;
	case IORING_OP_RECVMSG:
		ret = io_recvmsg(req, issue_flags);
		break;
	case IORING_OP_RECV:
		ret = io_recv(req, issue_flags);
		break;
	case IORING_OP_TIMEOUT:
		ret = io_timeout(req, issue_flags);
		break;
	case IORING_OP_TIMEOUT_REMOVE:
		ret = io_timeout_remove(req, issue_flags);
		break;
	case IORING_OP_ACCEPT:
		ret = io_accept(req, issue_flags);
		break;
	case IORING_OP_CONNECT:
		ret = io_connect(req, issue_flags);
		break;
	case IORING_OP_ASYNC_CANCEL:
		ret = io_async_cancel(req, issue_flags);
		break;
	case IORING_OP_FALLOCATE:
		ret = io_fallocate(req, issue_flags);
		break;
	case IORING_OP_OPENAT:
		ret = io_openat(req, issue_flags);
		break;
	case IORING_OP_CLOSE:
		ret = io_close(req, issue_flags);
		break;
	case IORING_OP_FILES_UPDATE:
		ret = io_files_update(req, issue_flags);
		break;
	case IORING_OP_STATX:
		ret = io_statx(req, issue_flags);
		break;
	case IORING_OP_FADVISE:
		ret = io_fadvise(req, issue_flags);
		break;
	case IORING_OP_MADVISE:
		ret = io_madvise(req, issue_flags);
		break;
	case IORING_OP_OPENAT2:
		ret = io_openat2(req, issue_flags);
		break;
	case IORING_OP_EPOLL_CTL:
		ret = io_epoll_ctl(req, issue_flags);
		break;
	case IORING_OP_SPLICE:
		ret = io_splice(req, issue_flags);
		break;
	case IORING_OP_PROVIDE_BUFFERS:
		ret = io_provide_buffers(req, issue_flags);
		break;
	case IORING_OP_REMOVE_BUFFERS:
		ret = io_remove_buffers(req, issue_flags);
		break;
	case IORING_OP_TEE:
		ret = io_tee(req, issue_flags);
		break;
	case IORING_OP_SHUTDOWN:
		ret = io_shutdown(req, issue_flags);
		break;
	case IORING_OP_RENAMEAT:
		ret = io_renameat(req, issue_flags);
		break;
	case IORING_OP_UNLINKAT:
		ret = io_unlinkat(req, issue_flags);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	if (creds)
		revert_creds(creds);

	if (ret)
		return ret;

	/* If the op doesn't have a file, we're not polling for it */
	if ((ctx->flags & IORING_SETUP_IOPOLL) && req->file) {
		const bool in_async = io_wq_current_is_worker();

		/* workqueue context doesn't hold uring_lock, grab it now */
		if (in_async)
			mutex_lock(&ctx->uring_lock);

		io_iopoll_req_issued(req, in_async);

		if (in_async)
			mutex_unlock(&ctx->uring_lock);
	}

	return 0;