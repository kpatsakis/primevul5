static int nfs4_verify_channel_attrs(struct nfs41_create_session_args *args,
				     struct nfs41_create_session_res *res)
{
	int ret;

	ret = nfs4_verify_fore_channel_attrs(args, res);
	if (ret)
		return ret;
	return nfs4_verify_back_channel_attrs(args, res);
}