static int tipc_ehdr_build(struct net *net, struct tipc_aead *aead,
			   u8 tx_key, struct sk_buff *skb,
			   struct tipc_crypto *__rx)
{
	struct tipc_msg *hdr = buf_msg(skb);
	struct tipc_ehdr *ehdr;
	u32 user = msg_user(hdr);
	u64 seqno;
	int ehsz;

	/* Make room for encryption header */
	ehsz = (user != LINK_CONFIG) ? EHDR_SIZE : EHDR_CFG_SIZE;
	WARN_ON(skb_headroom(skb) < ehsz);
	ehdr = (struct tipc_ehdr *)skb_push(skb, ehsz);

	/* Obtain a seqno first:
	 * Use the key seqno (= cluster wise) if dest is unknown or we're in
	 * cluster key mode, otherwise it's better for a per-peer seqno!
	 */
	if (!__rx || aead->mode == CLUSTER_KEY)
		seqno = atomic64_inc_return(&aead->seqno);
	else
		seqno = atomic64_inc_return(&__rx->sndnxt);

	/* Revoke the key if seqno is wrapped around */
	if (unlikely(!seqno))
		return tipc_crypto_key_revoke(net, tx_key);

	/* Word 1-2 */
	ehdr->seqno = cpu_to_be64(seqno);

	/* Words 0, 3- */
	ehdr->version = TIPC_EVERSION;
	ehdr->user = 0;
	ehdr->keepalive = 0;
	ehdr->tx_key = tx_key;
	ehdr->destined = (__rx) ? 1 : 0;
	ehdr->rx_key_active = (__rx) ? __rx->key.active : 0;
	ehdr->rx_nokey = (__rx) ? __rx->nokey : 0;
	ehdr->master_key = aead->crypto->key_master;
	ehdr->reserved_1 = 0;
	ehdr->reserved_2 = 0;

	switch (user) {
	case LINK_CONFIG:
		ehdr->user = LINK_CONFIG;
		memcpy(ehdr->id, tipc_own_id(net), NODE_ID_LEN);
		break;
	default:
		if (user == LINK_PROTOCOL && msg_type(hdr) == STATE_MSG) {
			ehdr->user = LINK_PROTOCOL;
			ehdr->keepalive = msg_is_keepalive(hdr);
		}
		ehdr->addr = hdr->hdr[3];
		break;
	}

	return ehsz;
}