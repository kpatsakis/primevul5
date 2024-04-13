}
EXPORT_SYMBOL_GPL(iscsi_dbg_trace);

static __init int iscsi_transport_init(void)
{
	int err;
	struct netlink_kernel_cfg cfg = {
		.groups	= 1,
		.input	= iscsi_if_rx,
	};
	printk(KERN_INFO "Loading iSCSI transport class v%s.\n",
		ISCSI_TRANSPORT_VERSION);

	atomic_set(&iscsi_session_nr, 0);

	err = class_register(&iscsi_transport_class);
	if (err)
		return err;

	err = class_register(&iscsi_endpoint_class);
	if (err)
		goto unregister_transport_class;

	err = class_register(&iscsi_iface_class);
	if (err)
		goto unregister_endpoint_class;

	err = transport_class_register(&iscsi_host_class);
	if (err)
		goto unregister_iface_class;

	err = transport_class_register(&iscsi_connection_class);
	if (err)
		goto unregister_host_class;

	err = transport_class_register(&iscsi_session_class);
	if (err)
		goto unregister_conn_class;

	err = bus_register(&iscsi_flashnode_bus);
	if (err)
		goto unregister_session_class;

	nls = netlink_kernel_create(&init_net, NETLINK_ISCSI, &cfg);
	if (!nls) {
		err = -ENOBUFS;
		goto unregister_flashnode_bus;
	}

	iscsi_eh_timer_workq = alloc_workqueue("%s",
			WQ_SYSFS | __WQ_LEGACY | WQ_MEM_RECLAIM | WQ_UNBOUND,
			1, "iscsi_eh");
	if (!iscsi_eh_timer_workq) {
		err = -ENOMEM;
		goto release_nls;
	}

	iscsi_destroy_workq = alloc_workqueue("%s",
			WQ_SYSFS | __WQ_LEGACY | WQ_MEM_RECLAIM | WQ_UNBOUND,
			1, "iscsi_destroy");
	if (!iscsi_destroy_workq) {
		err = -ENOMEM;
		goto destroy_wq;
	}

	return 0;

destroy_wq:
	destroy_workqueue(iscsi_eh_timer_workq);
release_nls:
	netlink_kernel_release(nls);
unregister_flashnode_bus:
	bus_unregister(&iscsi_flashnode_bus);
unregister_session_class:
	transport_class_unregister(&iscsi_session_class);
unregister_conn_class:
	transport_class_unregister(&iscsi_connection_class);
unregister_host_class:
	transport_class_unregister(&iscsi_host_class);
unregister_iface_class:
	class_unregister(&iscsi_iface_class);
unregister_endpoint_class:
	class_unregister(&iscsi_endpoint_class);
unregister_transport_class: