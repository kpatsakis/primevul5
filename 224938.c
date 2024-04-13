	NULL,
};

static void iscsi_flashnode_conn_release(struct device *dev)
{
	struct iscsi_bus_flash_conn *fnode_conn = iscsi_dev_to_flash_conn(dev);

	kfree(fnode_conn->ipaddress);
	kfree(fnode_conn->redirect_ipaddr);