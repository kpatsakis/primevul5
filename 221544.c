static noinline bool io_cqring_fill_event(struct io_ring_ctx *ctx, u64 user_data,
					  long res, unsigned int cflags)
{
	return __io_cqring_fill_event(ctx, user_data, res, cflags);
}