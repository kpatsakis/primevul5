struct fuse_dev *fuse_dev_alloc(void)
{
	struct fuse_dev *fud;
	struct list_head *pq;

	fud = kzalloc(sizeof(struct fuse_dev), GFP_KERNEL);
	if (!fud)
		return NULL;

	pq = kcalloc(FUSE_PQ_HASH_SIZE, sizeof(struct list_head), GFP_KERNEL);
	if (!pq) {
		kfree(fud);
		return NULL;
	}

	fud->pq.processing = pq;
	fuse_pqueue_init(&fud->pq);

	return fud;
}