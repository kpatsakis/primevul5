 * Deletes the flashnode session entry and all children flashnode connection
 * entries from sysfs
 */
void iscsi_destroy_flashnode_sess(struct iscsi_bus_flash_session *fnode_sess)
{
	int err;

	err = device_for_each_child(&fnode_sess->dev, NULL,
				    iscsi_iter_destroy_flashnode_conn_fn);
	if (err)
		pr_err("Could not delete all connections for %s. Error %d.\n",
		       fnode_sess->dev.kobj.name, err);