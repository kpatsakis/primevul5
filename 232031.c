 */
static unsigned int bfq_update_depths(struct bfq_data *bfqd,
				      struct sbitmap_queue *bt)
{
	unsigned int i, j, min_shallow = UINT_MAX;

	/*
	 * In-word depths if no bfq_queue is being weight-raised:
	 * leaving 25% of tags only for sync reads.
	 *
	 * In next formulas, right-shift the value
	 * (1U<<bt->sb.shift), instead of computing directly
	 * (1U<<(bt->sb.shift - something)), to be robust against
	 * any possible value of bt->sb.shift, without having to
	 * limit 'something'.
	 */
	/* no more than 50% of tags for async I/O */
	bfqd->word_depths[0][0] = max((1U << bt->sb.shift) >> 1, 1U);
	/*
	 * no more than 75% of tags for sync writes (25% extra tags
	 * w.r.t. async I/O, to prevent async I/O from starving sync
	 * writes)
	 */
	bfqd->word_depths[0][1] = max(((1U << bt->sb.shift) * 3) >> 2, 1U);

	/*
	 * In-word depths in case some bfq_queue is being weight-
	 * raised: leaving ~63% of tags for sync reads. This is the
	 * highest percentage for which, in our tests, application
	 * start-up times didn't suffer from any regression due to tag
	 * shortage.
	 */
	/* no more than ~18% of tags for async I/O */
	bfqd->word_depths[1][0] = max(((1U << bt->sb.shift) * 3) >> 4, 1U);
	/* no more than ~37% of tags for sync writes (~20% extra tags) */
	bfqd->word_depths[1][1] = max(((1U << bt->sb.shift) * 6) >> 4, 1U);

	for (i = 0; i < 2; i++)
		for (j = 0; j < 2; j++)
			min_shallow = min(min_shallow, bfqd->word_depths[i][j]);

	return min_shallow;