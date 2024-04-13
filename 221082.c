void eventpoll_release_file(struct file *file)
{
	struct eventpoll *ep;
	struct epitem *epi, *next;

	/*
	 * We don't want to get "file->f_lock" because it is not
	 * necessary. It is not necessary because we're in the "struct file"
	 * cleanup path, and this means that no one is using this file anymore.
	 * So, for example, epoll_ctl() cannot hit here since if we reach this
	 * point, the file counter already went to zero and fget() would fail.
	 * The only hit might come from ep_free() but by holding the mutex
	 * will correctly serialize the operation. We do need to acquire
	 * "ep->mtx" after "epmutex" because ep_remove() requires it when called
	 * from anywhere but ep_free().
	 *
	 * Besides, ep_remove() acquires the lock, so we can't hold it here.
	 */
	mutex_lock(&epmutex);
	list_for_each_entry_safe(epi, next, &file->f_ep_links, fllink) {
		ep = epi->ep;
		mutex_lock_nested(&ep->mtx, 0);
		ep_remove(ep, epi);
		mutex_unlock(&ep->mtx);
	}
	mutex_unlock(&epmutex);
}