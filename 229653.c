static bool _nfs4_is_integrity_protected(struct nfs_client *clp)
{
	rpc_authflavor_t flavor = clp->cl_rpcclient->cl_auth->au_flavor;
	return (flavor == RPC_AUTH_GSS_KRB5I) || (flavor == RPC_AUTH_GSS_KRB5P);
}