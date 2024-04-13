static void nfs4_layoutreturn_release(void *calldata)
{
	struct nfs4_layoutreturn *lrp = calldata;
	struct pnfs_layout_hdr *lo = lrp->args.layout;

	dprintk("--> %s\n", __func__);
	pnfs_layoutreturn_free_lsegs(lo, &lrp->args.stateid, &lrp->args.range,
			lrp->res.lrs_present ? &lrp->res.stateid : NULL);
	nfs4_sequence_free_slot(&lrp->res.seq_res);
	if (lrp->ld_private.ops && lrp->ld_private.ops->free)
		lrp->ld_private.ops->free(&lrp->ld_private);
	pnfs_put_layout_hdr(lrp->args.layout);
	nfs_iput_and_deactive(lrp->inode);
	put_cred(lrp->cred);
	kfree(calldata);
	dprintk("<-- %s\n", __func__);
}