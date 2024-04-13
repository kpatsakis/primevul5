static void cil_reset_rangetransition(struct cil_rangetransition *rangetrans)
{
	if (rangetrans->range_str == NULL) {
		cil_reset_levelrange(rangetrans->range);
	}
}