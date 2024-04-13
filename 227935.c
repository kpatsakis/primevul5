static __sum16 __tcp_checksum_complete_user(struct sock *sk,
					    struct sk_buff *skb)
{
	__sum16 result;

	if (sock_owned_by_user(sk)) {
		local_bh_enable();
		result = __tcp_checksum_complete(skb);
		local_bh_disable();
	} else {
		result = __tcp_checksum_complete(skb);
	}
	return result;
}