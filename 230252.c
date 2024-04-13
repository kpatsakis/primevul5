static void nfs4_slot_sequence_acked(struct nfs4_slot *slot,
		u32 seqnr)
{
	slot->seq_nr_highest_sent = seqnr;
	slot->seq_nr_last_acked = seqnr;
}