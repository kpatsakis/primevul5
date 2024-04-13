int prepare_binprm(struct linux_binprm *bprm)
{
	umode_t mode;
	struct inode * inode = bprm->file->f_path.dentry->d_inode;
	int retval;

	mode = inode->i_mode;
	if (bprm->file->f_op == NULL)
		return -EACCES;

	/* clear any previous set[ug]id data from a previous binary */
	bprm->cred->euid = current_euid();
	bprm->cred->egid = current_egid();

	if (!(bprm->file->f_path.mnt->mnt_flags & MNT_NOSUID) &&
	    !current->no_new_privs) {
		/* Set-uid? */
		if (mode & S_ISUID) {
			if (!kuid_has_mapping(bprm->cred->user_ns, inode->i_uid))
				return -EPERM;
			bprm->per_clear |= PER_CLEAR_ON_SETID;
			bprm->cred->euid = inode->i_uid;

		}

		/* Set-gid? */
		/*
		 * If setgid is set but no group execute bit then this
		 * is a candidate for mandatory locking, not a setgid
		 * executable.
		 */
		if ((mode & (S_ISGID | S_IXGRP)) == (S_ISGID | S_IXGRP)) {
			if (!kgid_has_mapping(bprm->cred->user_ns, inode->i_gid))
				return -EPERM;
			bprm->per_clear |= PER_CLEAR_ON_SETID;
			bprm->cred->egid = inode->i_gid;
		}
	}

	/* fill in binprm security blob */
	retval = security_bprm_set_creds(bprm);
	if (retval)
		return retval;
	bprm->cred_prepared = 1;

	memset(bprm->buf, 0, BINPRM_BUF_SIZE);
	return kernel_read(bprm->file, 0, bprm->buf, BINPRM_BUF_SIZE);
}