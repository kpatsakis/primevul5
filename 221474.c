static struct io_buffer *io_buffer_select(struct io_kiocb *req, size_t *len,
					  int bgid, struct io_buffer *kbuf,
					  bool needs_lock)
{
	struct io_buffer *head;

	if (req->flags & REQ_F_BUFFER_SELECTED)
		return kbuf;

	io_ring_submit_lock(req->ctx, needs_lock);

	lockdep_assert_held(&req->ctx->uring_lock);

	head = xa_load(&req->ctx->io_buffers, bgid);
	if (head) {
		if (!list_empty(&head->list)) {
			kbuf = list_last_entry(&head->list, struct io_buffer,
							list);
			list_del(&kbuf->list);
		} else {
			kbuf = head;
			xa_erase(&req->ctx->io_buffers, bgid);
		}
		if (*len > kbuf->len)
			*len = kbuf->len;
	} else {
		kbuf = ERR_PTR(-ENOBUFS);
	}

	io_ring_submit_unlock(req->ctx, needs_lock);

	return kbuf;
}