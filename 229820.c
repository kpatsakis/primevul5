static void nfs4_slot_sequence_record_sent(struct nfs4_slot *slot,
		u32 seqnr)
{
	if ((s32)(seqnr - slot->seq_nr_highest_sent) > 0)
		slot->seq_nr_highest_sent = seqnr;
}