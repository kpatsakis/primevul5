
static struct io_sq_data *io_attach_sq_data(struct io_uring_params *p)
{
	struct io_ring_ctx *ctx_attach;
	struct io_sq_data *sqd;
	struct fd f;

	f = fdget(p->wq_fd);
	if (!f.file)
		return ERR_PTR(-ENXIO);
	if (f.file->f_op != &io_uring_fops) {
		fdput(f);
		return ERR_PTR(-EINVAL);
	}

	ctx_attach = f.file->private_data;
	sqd = ctx_attach->sq_data;
	if (!sqd) {
		fdput(f);
		return ERR_PTR(-EINVAL);
	}
	if (sqd->task_tgid != current->tgid) {
		fdput(f);
		return ERR_PTR(-EPERM);
	}

	refcount_inc(&sqd->refs);
	fdput(f);
	return sqd;