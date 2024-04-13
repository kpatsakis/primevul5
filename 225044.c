
static struct bus_type iscsi_flashnode_bus;

int iscsi_flashnode_bus_match(struct device *dev,
				     struct device_driver *drv)
{
	if (dev->bus == &iscsi_flashnode_bus)