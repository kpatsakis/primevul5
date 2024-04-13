static int io_openat2_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct open_how __user *how;
	size_t len;
	int ret;

	if (req->flags & REQ_F_NEED_CLEANUP)
		return 0;
	how = u64_to_user_ptr(READ_ONCE(sqe->addr2));
	len = READ_ONCE(sqe->len);
	if (len < OPEN_HOW_SIZE_VER0)
		return -EINVAL;

	ret = copy_struct_from_user(&req->open.how, sizeof(req->open.how), how,
					len);
	if (ret)
		return ret;

	return __io_openat_prep(req, sqe);
}