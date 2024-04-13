static struct nfs4_lockdata *nfs4_alloc_lockdata(struct file_lock *fl,
		struct nfs_open_context *ctx, struct nfs4_lock_state *lsp,
		gfp_t gfp_mask)
{
	struct nfs4_lockdata *p;
	struct inode *inode = lsp->ls_state->inode;
	struct nfs_server *server = NFS_SERVER(inode);
	struct nfs_seqid *(*alloc_seqid)(struct nfs_seqid_counter *, gfp_t);

	p = kzalloc(sizeof(*p), gfp_mask);
	if (p == NULL)
		return NULL;

	p->arg.fh = NFS_FH(inode);
	p->arg.fl = &p->fl;
	p->arg.open_seqid = nfs_alloc_seqid(&lsp->ls_state->owner->so_seqid, gfp_mask);
	if (IS_ERR(p->arg.open_seqid))
		goto out_free;
	alloc_seqid = server->nfs_client->cl_mvops->alloc_seqid;
	p->arg.lock_seqid = alloc_seqid(&lsp->ls_seqid, gfp_mask);
	if (IS_ERR(p->arg.lock_seqid))
		goto out_free_seqid;
	p->arg.lock_owner.clientid = server->nfs_client->cl_clientid;
	p->arg.lock_owner.id = lsp->ls_seqid.owner_id;
	p->arg.lock_owner.s_dev = server->s_dev;
	p->res.lock_seqid = p->arg.lock_seqid;
	p->lsp = lsp;
	p->server = server;
	p->ctx = get_nfs_open_context(ctx);
	locks_init_lock(&p->fl);
	locks_copy_lock(&p->fl, fl);
	return p;
out_free_seqid:
	nfs_free_seqid(p->arg.open_seqid);
out_free:
	kfree(p);
	return NULL;
}