
static int io_req_prep_async(struct io_kiocb *req)
{
	if (!io_op_defs[req->opcode].needs_async_setup)
		return 0;
	if (WARN_ON_ONCE(req->async_data))
		return -EFAULT;
	if (io_alloc_async_data(req))
		return -EAGAIN;

	switch (req->opcode) {
	case IORING_OP_READV:
		return io_rw_prep_async(req, READ);
	case IORING_OP_WRITEV:
		return io_rw_prep_async(req, WRITE);
	case IORING_OP_SENDMSG:
		return io_sendmsg_prep_async(req);
	case IORING_OP_RECVMSG:
		return io_recvmsg_prep_async(req);
	case IORING_OP_CONNECT:
		return io_connect_prep_async(req);
	}
	printk_once(KERN_WARNING "io_uring: prep_async() bad opcode %d\n",
		    req->opcode);
	return -EFAULT;