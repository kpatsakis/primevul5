static int io_send(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_sr_msg *sr = &req->sr_msg;
	struct msghdr msg;
	struct iovec iov;
	struct socket *sock;
	unsigned flags;
	int min_ret = 0;
	int ret;

	sock = sock_from_file(req->file);
	if (unlikely(!sock))
		return -ENOTSOCK;

	ret = import_single_range(WRITE, sr->buf, sr->len, &iov, &msg.msg_iter);
	if (unlikely(ret))
		return ret;

	msg.msg_name = NULL;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_namelen = 0;

	flags = req->sr_msg.msg_flags;
	if (issue_flags & IO_URING_F_NONBLOCK)
		flags |= MSG_DONTWAIT;
	if (flags & MSG_WAITALL)
		min_ret = iov_iter_count(&msg.msg_iter);

	msg.msg_flags = flags;
	ret = sock_sendmsg(sock, &msg);
	if ((issue_flags & IO_URING_F_NONBLOCK) && ret == -EAGAIN)
		return -EAGAIN;
	if (ret == -ERESTARTSYS)
		ret = -EINTR;

	if (ret < min_ret)
		req_set_fail_links(req);
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
}