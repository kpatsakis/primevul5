static int io_recv(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_buffer *kbuf;
	struct io_sr_msg *sr = &req->sr_msg;
	struct msghdr msg;
	void __user *buf = sr->buf;
	struct socket *sock;
	struct iovec iov;
	unsigned flags;
	int min_ret = 0;
	int ret, cflags = 0;
	bool force_nonblock = issue_flags & IO_URING_F_NONBLOCK;

	sock = sock_from_file(req->file);
	if (unlikely(!sock))
		return -ENOTSOCK;

	if (req->flags & REQ_F_BUFFER_SELECT) {
		kbuf = io_recv_buffer_select(req, !force_nonblock);
		if (IS_ERR(kbuf))
			return PTR_ERR(kbuf);
		buf = u64_to_user_ptr(kbuf->addr);
	}

	ret = import_single_range(READ, buf, sr->len, &iov, &msg.msg_iter);
	if (unlikely(ret))
		goto out_free;

	msg.msg_name = NULL;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_namelen = 0;
	msg.msg_iocb = NULL;
	msg.msg_flags = 0;

	flags = req->sr_msg.msg_flags;
	if (force_nonblock)
		flags |= MSG_DONTWAIT;
	if (flags & MSG_WAITALL)
		min_ret = iov_iter_count(&msg.msg_iter);

	ret = sock_recvmsg(sock, &msg, flags);
	if (force_nonblock && ret == -EAGAIN)
		return -EAGAIN;
	if (ret == -ERESTARTSYS)
		ret = -EINTR;
out_free:
	if (req->flags & REQ_F_BUFFER_SELECTED)
		cflags = io_put_recv_kbuf(req);
	if (ret < min_ret || ((flags & MSG_WAITALL) && (msg.msg_flags & (MSG_TRUNC | MSG_CTRUNC))))
		req_set_fail_links(req);
	__io_req_complete(req, issue_flags, ret, cflags);
	return 0;
}