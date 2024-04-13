 *  %NULL on failure
 */
struct iscsi_bus_flash_conn *
iscsi_create_flashnode_conn(struct Scsi_Host *shost,
			    struct iscsi_bus_flash_session *fnode_sess,
			    struct iscsi_transport *transport,
			    int dd_size)
{
	struct iscsi_bus_flash_conn *fnode_conn;
	int err;

	fnode_conn = kzalloc(sizeof(*fnode_conn) + dd_size, GFP_KERNEL);
	if (!fnode_conn)
		return NULL;

	fnode_conn->transport = transport;
	fnode_conn->dev.type = &iscsi_flashnode_conn_dev_type;
	fnode_conn->dev.bus = &iscsi_flashnode_bus;
	fnode_conn->dev.parent = &fnode_sess->dev;
	dev_set_name(&fnode_conn->dev, "flashnode_conn-%u:%u:0",
		     shost->host_no, fnode_sess->target_id);

	err = device_register(&fnode_conn->dev);
	if (err)
		goto free_fnode_conn;

	if (dd_size)
		fnode_conn->dd_data = &fnode_conn[1];

	return fnode_conn;

free_fnode_conn: