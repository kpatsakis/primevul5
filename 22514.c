int cgroup1_get_tree(struct fs_context *fc)
{
	struct cgroup_fs_context *ctx = cgroup_fc2context(fc);
	int ret;

	/* Check if the caller has permission to mount. */
	if (!ns_capable(ctx->ns->user_ns, CAP_SYS_ADMIN))
		return -EPERM;

	cgroup_lock_and_drain_offline(&cgrp_dfl_root.cgrp);

	ret = cgroup1_root_to_use(fc);
	if (!ret && !percpu_ref_tryget_live(&ctx->root->cgrp.self.refcnt))
		ret = 1;	/* restart */

	mutex_unlock(&cgroup_mutex);

	if (!ret)
		ret = cgroup_do_get_tree(fc);

	if (!ret && percpu_ref_is_dying(&ctx->root->cgrp.self.refcnt)) {
		struct super_block *sb = fc->root->d_sb;
		dput(fc->root);
		deactivate_locked_super(sb);
		ret = 1;
	}

	if (unlikely(ret > 0)) {
		msleep(10);
		return restart_syscall();
	}
	return ret;
}