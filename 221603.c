
static int io_queue_rsrc_removal(struct io_rsrc_data *data, unsigned idx,
				 struct io_rsrc_node *node, void *rsrc)
{
	struct io_rsrc_put *prsrc;

	prsrc = kzalloc(sizeof(*prsrc), GFP_KERNEL);
	if (!prsrc)
		return -ENOMEM;

	prsrc->tag = data->tags[idx];
	prsrc->rsrc = rsrc;
	list_add(&prsrc->list, &node->rsrc_list);
	return 0;