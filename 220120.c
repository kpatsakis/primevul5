void nfc_send_to_raw_sock(struct nfc_dev *dev, struct sk_buff *skb,
			  u8 payload_type, u8 direction)
{
	struct sk_buff *skb_copy = NULL, *nskb;
	struct sock *sk;
	u8 *data;

	read_lock(&raw_sk_list.lock);

	sk_for_each(sk, &raw_sk_list.head) {
		if (!skb_copy) {
			skb_copy = __pskb_copy_fclone(skb, NFC_RAW_HEADER_SIZE,
						      GFP_ATOMIC, true);
			if (!skb_copy)
				continue;

			data = skb_push(skb_copy, NFC_RAW_HEADER_SIZE);

			data[0] = dev ? dev->idx : 0xFF;
			data[1] = direction & 0x01;
			data[1] |= (payload_type << 1);
		}

		nskb = skb_clone(skb_copy, GFP_ATOMIC);
		if (!nskb)
			continue;

		if (sock_queue_rcv_skb(sk, nskb))
			kfree_skb(nskb);
	}

	read_unlock(&raw_sk_list.lock);

	kfree_skb(skb_copy);
}