 *  %NULL on failure
 */
struct iscsi_bus_flash_session *
iscsi_create_flashnode_sess(struct Scsi_Host *shost, int index,
			    struct iscsi_transport *transport,
			    int dd_size)
{
	struct iscsi_bus_flash_session *fnode_sess;
	int err;

	fnode_sess = kzalloc(sizeof(*fnode_sess) + dd_size, GFP_KERNEL);
	if (!fnode_sess)
		return NULL;

	fnode_sess->transport = transport;
	fnode_sess->target_id = index;
	fnode_sess->dev.type = &iscsi_flashnode_sess_dev_type;
	fnode_sess->dev.bus = &iscsi_flashnode_bus;
	fnode_sess->dev.parent = &shost->shost_gendev;
	dev_set_name(&fnode_sess->dev, "flashnode_sess-%u:%u",
		     shost->host_no, index);

	err = device_register(&fnode_sess->dev);
	if (err)
		goto free_fnode_sess;

	if (dd_size)
		fnode_sess->dd_data = &fnode_sess[1];

	return fnode_sess;

free_fnode_sess: