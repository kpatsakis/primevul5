void nfs4_sequence_attach_slot(struct nfs4_sequence_args *args,
		struct nfs4_sequence_res *res,
		struct nfs4_slot *slot)
{
	if (!slot)
		return;
	slot->privileged = args->sa_privileged ? 1 : 0;
	args->sa_slot = slot;

	res->sr_slot = slot;
}