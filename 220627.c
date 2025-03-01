static int __net_init packet_net_init(struct net *net)
{
	mutex_init(&net->packet.sklist_lock);
	INIT_HLIST_HEAD(&net->packet.sklist);

	if (!proc_create_net("packet", 0, net->proc_net, &packet_seq_ops,
			sizeof(struct seq_net_private)))
		return -ENOMEM;

	return 0;
}