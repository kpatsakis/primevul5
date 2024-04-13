static int io_close(struct io_kiocb *req, unsigned int issue_flags)
{
	struct files_struct *files = current->files;
	struct io_close *close = &req->close;
	struct fdtable *fdt;
	struct file *file = NULL;
	int ret = -EBADF;

	spin_lock(&files->file_lock);
	fdt = files_fdtable(files);
	if (close->fd >= fdt->max_fds) {
		spin_unlock(&files->file_lock);
		goto err;
	}
	file = fdt->fd[close->fd];
	if (!file || file->f_op == &io_uring_fops) {
		spin_unlock(&files->file_lock);
		file = NULL;
		goto err;
	}

	/* if the file has a flush method, be safe and punt to async */
	if (file->f_op->flush && (issue_flags & IO_URING_F_NONBLOCK)) {
		spin_unlock(&files->file_lock);
		return -EAGAIN;
	}

	ret = __close_fd_get_file(close->fd, &file);
	spin_unlock(&files->file_lock);
	if (ret < 0) {
		if (ret == -ENOENT)
			ret = -EBADF;
		goto err;
	}

	/* No ->flush() or already async, safely close from here */
	ret = filp_close(file, current->files);
err:
	if (ret < 0)
		req_set_fail_links(req);
	if (file)
		fput(file);
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
}