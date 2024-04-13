int nfs4_proc_exchange_id(struct nfs_client *clp, const struct cred *cred)
{
	rpc_authflavor_t authflavor = clp->cl_rpcclient->cl_auth->au_flavor;
	int status;

	/* try SP4_MACH_CRED if krb5i/p	*/
	if (authflavor == RPC_AUTH_GSS_KRB5I ||
	    authflavor == RPC_AUTH_GSS_KRB5P) {
		status = _nfs4_proc_exchange_id(clp, cred, SP4_MACH_CRED);
		if (!status)
			return 0;
	}

	/* try SP4_NONE */
	return _nfs4_proc_exchange_id(clp, cred, SP4_NONE);
}