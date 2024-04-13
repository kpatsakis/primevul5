static void nfs4_layoutget_release(void *calldata)
{
	struct nfs4_layoutget *lgp = calldata;

	dprintk("--> %s\n", __func__);
	nfs4_sequence_free_slot(&lgp->res.seq_res);
	pnfs_layoutget_free(lgp);
	dprintk("<-- %s\n", __func__);
}