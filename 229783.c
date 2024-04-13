static void nfs41_sequence_res_init(struct nfs4_sequence_res *res)
{
	res->sr_timestamp = jiffies;
	res->sr_status_flags = 0;
	res->sr_status = 1;
}