static void nfs4_init_boot_verifier(const struct nfs_client *clp,
				    nfs4_verifier *bootverf)
{
	__be32 verf[2];

	if (test_bit(NFS4CLNT_PURGE_STATE, &clp->cl_state)) {
		/* An impossible timestamp guarantees this value
		 * will never match a generated boot time. */
		verf[0] = cpu_to_be32(U32_MAX);
		verf[1] = cpu_to_be32(U32_MAX);
	} else {
		struct nfs_net *nn = net_generic(clp->cl_net, nfs_net_id);
		u64 ns = ktime_to_ns(nn->boot_time);

		verf[0] = cpu_to_be32(ns >> 32);
		verf[1] = cpu_to_be32(ns);
	}
	memcpy(bootverf->data, verf, sizeof(bootverf->data));
}