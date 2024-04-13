static void nfs4_sequence_free_slot(struct nfs4_sequence_res *res)
{
	if (res->sr_slot != NULL) {
		if (res->sr_slot->table->session != NULL)
			nfs41_sequence_free_slot(res);
		else
			nfs40_sequence_free_slot(res);
	}
}