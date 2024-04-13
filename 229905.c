static void nfs41_sequence_free_slot(struct nfs4_sequence_res *res)
{
	nfs41_release_slot(res->sr_slot);
	res->sr_slot = NULL;
}