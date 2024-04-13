 *  %NULL on failure
 */
static struct iscsi_bus_flash_session *
iscsi_get_flashnode_by_index(struct Scsi_Host *shost, uint32_t idx)
{
	struct iscsi_bus_flash_session *fnode_sess = NULL;
	struct device *dev;

	dev = device_find_child(&shost->shost_gendev, &idx,
				flashnode_match_index);
	if (dev)
		fnode_sess = iscsi_dev_to_flash_session(dev);