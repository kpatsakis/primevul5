	return 0;
}

static int flashnode_match_index(struct device *dev, void *data)
{
	struct iscsi_bus_flash_session *fnode_sess = NULL;
	int ret = 0;

	if (!iscsi_flashnode_bus_match(dev, NULL))
		goto exit_match_index;

	fnode_sess = iscsi_dev_to_flash_session(dev);
	ret = (fnode_sess->target_id == *((int *)data)) ? 1 : 0;
