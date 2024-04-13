	return err;
}

static void __exit iscsi_transport_exit(void)
{
	destroy_workqueue(iscsi_destroy_workq);
	destroy_workqueue(iscsi_eh_timer_workq);
	netlink_kernel_release(nls);
	bus_unregister(&iscsi_flashnode_bus);
	transport_class_unregister(&iscsi_connection_class);
	transport_class_unregister(&iscsi_session_class);
	transport_class_unregister(&iscsi_host_class);
	class_unregister(&iscsi_endpoint_class);