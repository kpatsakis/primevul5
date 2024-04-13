	return dev->bus == &iscsi_flashnode_bus;
}

static int iscsi_destroy_flashnode_conn(struct iscsi_bus_flash_conn *fnode_conn)
{