static int xfrm6_tunnel_rcv(struct sk_buff *skb)
{
	struct ipv6hdr *iph = ipv6_hdr(skb);
 	__be32 spi;
 
 	spi = xfrm6_tunnel_spi_lookup((xfrm_address_t *)&iph->saddr);
	return xfrm6_rcv_spi(skb, spi);
 }
