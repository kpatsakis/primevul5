static void fuse_pqueue_init(struct fuse_pqueue *fpq)
{
	unsigned int i;

	spin_lock_init(&fpq->lock);
	for (i = 0; i < FUSE_PQ_HASH_SIZE; i++)
		INIT_LIST_HEAD(&fpq->processing[i]);
	INIT_LIST_HEAD(&fpq->io);
	fpq->connected = 1;
}