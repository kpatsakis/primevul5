bool tipc_ehdr_validate(struct sk_buff *skb)
{
	struct tipc_ehdr *ehdr;
	int ehsz;

	if (unlikely(!pskb_may_pull(skb, EHDR_MIN_SIZE)))
		return false;

	ehdr = (struct tipc_ehdr *)skb->data;
	if (unlikely(ehdr->version != TIPC_EVERSION))
		return false;
	ehsz = tipc_ehdr_size(ehdr);
	if (unlikely(!pskb_may_pull(skb, ehsz)))
		return false;
	if (unlikely(skb->len <= ehsz + TIPC_AES_GCM_TAG_SIZE))
		return false;

	return true;
}