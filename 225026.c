};

static void iscsi_flashnode_sess_release(struct device *dev)
{
	struct iscsi_bus_flash_session *fnode_sess =
						iscsi_dev_to_flash_session(dev);

	kfree(fnode_sess->targetname);
	kfree(fnode_sess->targetalias);
	kfree(fnode_sess->portal_type);