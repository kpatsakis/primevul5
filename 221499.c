
static bool io_wait_rsrc_data(struct io_rsrc_data *data)
{
	if (!data)
		return false;
	if (!atomic_dec_and_test(&data->refs))
		wait_for_completion(&data->done);
	return true;