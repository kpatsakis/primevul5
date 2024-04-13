static struct nfs4_unlockdata *nfs4_alloc_unlockdata(struct file_lock *fl,
		struct nfs_open_context *ctx,
		struct nfs4_lock_state *lsp,
		struct nfs_seqid *seqid)
{
	struct nfs4_unlockdata *p;
	struct nfs4_state *state = lsp->ls_state;
	struct inode *inode = state->inode;

	p = kzalloc(sizeof(*p), GFP_NOFS);
	if (p == NULL)
		return NULL;
	p->arg.fh = NFS_FH(inode);
	p->arg.fl = &p->fl;
	p->arg.seqid = seqid;
	p->res.seqid = seqid;
	p->lsp = lsp;
	/* Ensure we don't close file until we're done freeing locks! */
	p->ctx = get_nfs_open_context(ctx);
	p->l_ctx = nfs_get_lock_context(ctx);
	locks_init_lock(&p->fl);
	locks_copy_lock(&p->fl, fl);
	p->server = NFS_SERVER(inode);
	spin_lock(&state->state_lock);
	nfs4_stateid_copy(&p->arg.stateid, &lsp->ls_stateid);
	spin_unlock(&state->state_lock);
	return p;
}