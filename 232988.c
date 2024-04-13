static bool udp_lib_exact_dif_match(struct net *net, struct sk_buff *skb)
{
#if IS_ENABLED(CONFIG_NET_L3_MASTER_DEV)
	if (!net->ipv4.sysctl_udp_l3mdev_accept &&
	    skb && ipv4_l3mdev_skb(IPCB(skb)->flags))
		return true;
#endif
	return false;
}