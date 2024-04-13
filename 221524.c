
static struct io_rsrc_data *io_rsrc_data_alloc(struct io_ring_ctx *ctx,
					       rsrc_put_fn *do_put,
					       unsigned nr)
{
	struct io_rsrc_data *data;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return NULL;

	data->tags = kvcalloc(nr, sizeof(*data->tags), GFP_KERNEL);
	if (!data->tags) {
		kfree(data);
		return NULL;
	}

	atomic_set(&data->refs, 1);
	data->ctx = ctx;
	data->do_put = do_put;
	init_completion(&data->done);
	return data;