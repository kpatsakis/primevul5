}

struct scsi_transport_template *
iscsi_register_transport(struct iscsi_transport *tt)
{
	struct iscsi_internal *priv;
	unsigned long flags;
	int err;

	BUG_ON(!tt);

	priv = iscsi_if_transport_lookup(tt);
	if (priv)
		return NULL;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return NULL;
	INIT_LIST_HEAD(&priv->list);
	priv->iscsi_transport = tt;
	priv->t.user_scan = iscsi_user_scan;
	priv->t.create_work_queue = 1;

	priv->dev.class = &iscsi_transport_class;
	dev_set_name(&priv->dev, "%s", tt->name);
	err = device_register(&priv->dev);
	if (err)
		goto free_priv;

	err = sysfs_create_group(&priv->dev.kobj, &iscsi_transport_group);
	if (err)
		goto unregister_dev;

	/* host parameters */
	priv->t.host_attrs.ac.class = &iscsi_host_class.class;
	priv->t.host_attrs.ac.match = iscsi_host_match;
	priv->t.host_attrs.ac.grp = &iscsi_host_group;
	priv->t.host_size = sizeof(struct iscsi_cls_host);
	transport_container_register(&priv->t.host_attrs);

	/* connection parameters */
	priv->conn_cont.ac.class = &iscsi_connection_class.class;
	priv->conn_cont.ac.match = iscsi_conn_match;
	priv->conn_cont.ac.grp = &iscsi_conn_group;
	transport_container_register(&priv->conn_cont);

	/* session parameters */
	priv->session_cont.ac.class = &iscsi_session_class.class;
	priv->session_cont.ac.match = iscsi_session_match;
	priv->session_cont.ac.grp = &iscsi_session_group;
	transport_container_register(&priv->session_cont);

	spin_lock_irqsave(&iscsi_transport_lock, flags);
	list_add(&priv->list, &iscsi_transports);
	spin_unlock_irqrestore(&iscsi_transport_lock, flags);

	printk(KERN_NOTICE "iscsi: registered transport (%s)\n", tt->name);
	return &priv->t;

unregister_dev:
	device_unregister(&priv->dev);
	return NULL;
free_priv: