static void nfs4_sequence_free_slot(struct nfs4_sequence_res *res)
{
	if (res->sr_slot != NULL)
		nfs40_sequence_free_slot(res);
}