
static void io_buffer_unmap(struct io_ring_ctx *ctx, struct io_mapped_ubuf **slot)
{
	struct io_mapped_ubuf *imu = *slot;
	unsigned int i;

	if (imu != ctx->dummy_ubuf) {
		for (i = 0; i < imu->nr_bvecs; i++)
			unpin_user_page(imu->bvec[i].bv_page);
		if (imu->acct_pages)
			io_unaccount_mem(ctx, imu->acct_pages);
		kvfree(imu);
	}
	*slot = NULL;