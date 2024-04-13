
static inline struct file *io_file_from_index(struct io_ring_ctx *ctx,
					      int index)
{
	struct io_fixed_file *slot = io_fixed_file_slot(&ctx->file_table, index);

	return (struct file *) (slot->file_ptr & FFS_MASK);