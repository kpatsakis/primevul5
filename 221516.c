
static void io_rsrc_data_free(struct io_rsrc_data *data)
{
	kvfree(data->tags);
	kfree(data);