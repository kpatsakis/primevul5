static int __io_compat_recvmsg_copy_hdr(struct io_kiocb *req,
					struct io_async_msghdr *iomsg)
{
	struct io_sr_msg *sr = &req->sr_msg;
	struct compat_iovec __user *uiov;
	compat_uptr_t ptr;
	compat_size_t len;
	int ret;

	ret = __get_compat_msghdr(&iomsg->msg, sr->umsg_compat, &iomsg->uaddr,
				  &ptr, &len);
	if (ret)
		return ret;

	uiov = compat_ptr(ptr);
	if (req->flags & REQ_F_BUFFER_SELECT) {
		compat_ssize_t clen;

		if (len > 1)
			return -EINVAL;
		if (!access_ok(uiov, sizeof(*uiov)))
			return -EFAULT;
		if (__get_user(clen, &uiov->iov_len))
			return -EFAULT;
		if (clen < 0)
			return -EINVAL;
		sr->len = clen;
		iomsg->free_iov = NULL;
	} else {
		iomsg->free_iov = iomsg->fast_iov;
		ret = __import_iovec(READ, (struct iovec __user *)uiov, len,
				   UIO_FASTIOV, &iomsg->free_iov,
				   &iomsg->msg.msg_iter, true);
		if (ret < 0)
			return ret;
	}

	return 0;
}