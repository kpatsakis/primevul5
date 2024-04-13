static int io_sendmsg(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_async_msghdr iomsg, *kmsg;
	struct socket *sock;
	unsigned flags;
	int min_ret = 0;
	int ret;

	sock = sock_from_file(req->file);
	if (unlikely(!sock))
		return -ENOTSOCK;

	kmsg = req->async_data;
	if (!kmsg) {
		ret = io_sendmsg_copy_hdr(req, &iomsg);
		if (ret)
			return ret;
		kmsg = &iomsg;
	}

	flags = req->sr_msg.msg_flags;
	if (issue_flags & IO_URING_F_NONBLOCK)
		flags |= MSG_DONTWAIT;
	if (flags & MSG_WAITALL)
		min_ret = iov_iter_count(&kmsg->msg.msg_iter);

	ret = __sys_sendmsg_sock(sock, &kmsg->msg, flags);
	if ((issue_flags & IO_URING_F_NONBLOCK) && ret == -EAGAIN)
		return io_setup_async_msg(req, kmsg);
	if (ret == -ERESTARTSYS)
		ret = -EINTR;

	/* fast path, check for non-NULL to avoid function call */
	if (kmsg->free_iov)
		kfree(kmsg->free_iov);
	req->flags &= ~REQ_F_NEED_CLEANUP;
	if (ret < min_ret)
		req_set_fail_links(req);
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
}