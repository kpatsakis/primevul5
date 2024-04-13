struct file *get_epoll_tfile_raw_ptr(struct file *file, int tfd,
				     unsigned long toff)
{
	struct file *file_raw;
	struct eventpoll *ep;
	struct epitem *epi;

	if (!is_file_epoll(file))
		return ERR_PTR(-EINVAL);

	ep = file->private_data;

	mutex_lock(&ep->mtx);
	epi = ep_find_tfd(ep, tfd, toff);
	if (epi)
		file_raw = epi->ffd.file;
	else
		file_raw = ERR_PTR(-ENOENT);
	mutex_unlock(&ep->mtx);

	return file_raw;
}