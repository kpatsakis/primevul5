static void io_prep_async_work(struct io_kiocb *req)
{
	const struct io_op_def *def = &io_op_defs[req->opcode];

	io_req_init_async(req);

	if (req->flags & REQ_F_ISREG) {
		if (def->hash_reg_file)
			io_wq_hash_work(&req->work, file_inode(req->file));
	} else {
		if (def->unbound_nonreg_file)
			req->work.flags |= IO_WQ_WORK_UNBOUND;
	}
	if (!req->work.mm && def->needs_mm) {
		mmgrab(current->mm);
		req->work.mm = current->mm;
	}
	if (!req->work.creds)
		req->work.creds = get_current_cred();
	if (!req->work.fs && def->needs_fs) {
		spin_lock(&current->fs->lock);
		if (!current->fs->in_exec) {
			req->work.fs = current->fs;
			req->work.fs->users++;
		} else {
			req->work.flags |= IO_WQ_WORK_CANCEL;
		}
		spin_unlock(&current->fs->lock);
	}
	if (def->needs_fsize)
		req->work.fsize = rlimit(RLIMIT_FSIZE);
	else
		req->work.fsize = RLIM_INFINITY;
}