
void bfq_end_wr_async_queues(struct bfq_data *bfqd,
			     struct bfq_group *bfqg)
{
	int i, j;

	for (i = 0; i < 2; i++)
		for (j = 0; j < IOPRIO_BE_NR; j++)
			if (bfqg->async_bfqq[i][j])
				bfq_bfqq_end_wr(bfqg->async_bfqq[i][j]);
	if (bfqg->async_idle_bfqq)
		bfq_bfqq_end_wr(bfqg->async_idle_bfqq);