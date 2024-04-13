static void sctp_clear_owner_w(struct sctp_chunk *chunk)
{
	skb_orphan(chunk->skb);
}