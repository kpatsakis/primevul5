static size_t init_iov_iter(struct vhost_virtqueue *vq, struct iov_iter *iter,
			    size_t hdr_size, int out)
{
	/* Skip header. TODO: support TSO. */
	size_t len = iov_length(vq->iov, out);

	iov_iter_init(iter, WRITE, vq->iov, out, len);
	iov_iter_advance(iter, hdr_size);

	return iov_iter_count(iter);
}