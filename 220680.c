static void packet_parse_headers(struct sk_buff *skb, struct socket *sock)
{
	if ((!skb->protocol || skb->protocol == htons(ETH_P_ALL)) &&
	    sock->type == SOCK_RAW) {
		skb_reset_mac_header(skb);
		skb->protocol = dev_parse_header_protocol(skb);
	}

	skb_probe_transport_header(skb);
}