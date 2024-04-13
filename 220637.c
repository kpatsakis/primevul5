static int __init packet_init(void)
{
	int rc;

	rc = proto_register(&packet_proto, 0);
	if (rc)
		goto out;
	rc = sock_register(&packet_family_ops);
	if (rc)
		goto out_proto;
	rc = register_pernet_subsys(&packet_net_ops);
	if (rc)
		goto out_sock;
	rc = register_netdevice_notifier(&packet_netdev_notifier);
	if (rc)
		goto out_pernet;

	return 0;

out_pernet:
	unregister_pernet_subsys(&packet_net_ops);
out_sock:
	sock_unregister(PF_PACKET);
out_proto:
	proto_unregister(&packet_proto);
out:
	return rc;
}