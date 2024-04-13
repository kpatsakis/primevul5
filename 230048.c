static int nfs4_verify_back_channel_attrs(struct nfs41_create_session_args *args,
		struct nfs41_create_session_res *res)
{
	struct nfs4_channel_attrs *sent = &args->bc_attrs;
	struct nfs4_channel_attrs *rcvd = &res->bc_attrs;

	if (!(res->flags & SESSION4_BACK_CHAN))
		goto out;
	if (rcvd->max_rqst_sz > sent->max_rqst_sz)
		return -EINVAL;
	if (rcvd->max_resp_sz < sent->max_resp_sz)
		return -EINVAL;
	if (rcvd->max_resp_sz_cached > sent->max_resp_sz_cached)
		return -EINVAL;
	if (rcvd->max_ops > sent->max_ops)
		return -EINVAL;
	if (rcvd->max_reqs > sent->max_reqs)
		return -EINVAL;
out:
	return 0;
}