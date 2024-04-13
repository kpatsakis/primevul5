void free_sums(struct sum_struct *s)
{
	if (s->sums) free(s->sums);
	free(s);
}
