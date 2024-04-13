static inline bool __io_cqring_fill_event(struct io_ring_ctx *ctx, u64 user_data,
					  long res, unsigned int cflags)
{
	struct io_uring_cqe *cqe;

	trace_io_uring_complete(ctx, user_data, res, cflags);

	/*
	 * If we can't get a cq entry, userspace overflowed the
	 * submission (by quite a lot). Increment the overflow count in
	 * the ring.
	 */
	cqe = io_get_cqring(ctx);
	if (likely(cqe)) {
		WRITE_ONCE(cqe->user_data, user_data);
		WRITE_ONCE(cqe->res, res);
		WRITE_ONCE(cqe->flags, cflags);
		return true;
	}
	return io_cqring_event_overflow(ctx, user_data, res, cflags);
}