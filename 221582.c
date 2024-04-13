static bool io_file_supports_async(struct io_kiocb *req, int rw)
{
	if (rw == READ && (req->flags & REQ_F_ASYNC_READ))
		return true;
	else if (rw == WRITE && (req->flags & REQ_F_ASYNC_WRITE))
		return true;

	return __io_file_supports_async(req->file, rw);
}