static int __init x25_init(void)
{
	int rc;

	rc = proto_register(&x25_proto, 0);
	if (rc)
		goto out;

	rc = sock_register(&x25_family_ops);
	if (rc)
		goto out_proto;

	dev_add_pack(&x25_packet_type);

	rc = register_netdevice_notifier(&x25_dev_notifier);
	if (rc)
		goto out_sock;

	rc = x25_register_sysctl();
	if (rc)
		goto out_dev;

	rc = x25_proc_init();
	if (rc)
		goto out_sysctl;

	pr_info("Linux Version 0.2\n");

out:
	return rc;
out_sysctl:
	x25_unregister_sysctl();
out_dev:
	unregister_netdevice_notifier(&x25_dev_notifier);
out_sock:
	dev_remove_pack(&x25_packet_type);
	sock_unregister(AF_X25);
out_proto:
	proto_unregister(&x25_proto);
	goto out;
}