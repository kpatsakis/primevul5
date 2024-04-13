static int tipc_crypto_key_xmit(struct net *net, struct tipc_aead_key *skey,
				u16 gen, u8 mode, u32 dnode)
{
	struct sk_buff_head pkts;
	struct tipc_msg *hdr;
	struct sk_buff *skb;
	u16 size, cong_link_cnt;
	u8 *data;
	int rc;

	size = tipc_aead_key_size(skey);
	skb = tipc_buf_acquire(INT_H_SIZE + size, GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	hdr = buf_msg(skb);
	tipc_msg_init(tipc_own_addr(net), hdr, MSG_CRYPTO, KEY_DISTR_MSG,
		      INT_H_SIZE, dnode);
	msg_set_size(hdr, INT_H_SIZE + size);
	msg_set_key_gen(hdr, gen);
	msg_set_key_mode(hdr, mode);

	data = msg_data(hdr);
	*((__be32 *)(data + TIPC_AEAD_ALG_NAME)) = htonl(skey->keylen);
	memcpy(data, skey->alg_name, TIPC_AEAD_ALG_NAME);
	memcpy(data + TIPC_AEAD_ALG_NAME + sizeof(__be32), skey->key,
	       skey->keylen);

	__skb_queue_head_init(&pkts);
	__skb_queue_tail(&pkts, skb);
	if (dnode)
		rc = tipc_node_xmit(net, &pkts, dnode, 0);
	else
		rc = tipc_bcast_xmit(net, &pkts, &cong_link_cnt);

	return rc;
}