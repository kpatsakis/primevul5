struct iscsi_iface *
iscsi_create_iface(struct Scsi_Host *shost, struct iscsi_transport *transport,
		   uint32_t iface_type, uint32_t iface_num, int dd_size)
{
	struct iscsi_iface *iface;
	int err;

	iface = kzalloc(sizeof(*iface) + dd_size, GFP_KERNEL);
	if (!iface)
		return NULL;

	iface->transport = transport;
	iface->iface_type = iface_type;
	iface->iface_num = iface_num;
	iface->dev.release = iscsi_iface_release;
	iface->dev.class = &iscsi_iface_class;
	/* parent reference released in iscsi_iface_release */
	iface->dev.parent = get_device(&shost->shost_gendev);
	if (iface_type == ISCSI_IFACE_TYPE_IPV4)
		dev_set_name(&iface->dev, "ipv4-iface-%u-%u", shost->host_no,
			     iface_num);
	else
		dev_set_name(&iface->dev, "ipv6-iface-%u-%u", shost->host_no,
			     iface_num);

	err = device_register(&iface->dev);
	if (err)
		goto free_iface;

	err = sysfs_create_group(&iface->dev.kobj, &iscsi_iface_group);
	if (err)
		goto unreg_iface;

	if (dd_size)
		iface->dd_data = &iface[1];
	return iface;

unreg_iface:
	device_unregister(&iface->dev);
	return NULL;

free_iface:
	put_device(iface->dev.parent);
	kfree(iface);
	return NULL;