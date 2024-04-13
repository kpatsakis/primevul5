static int lp_preempt(void *handle)
{
	struct lp_struct *this_lp = (struct lp_struct *)handle;
	set_bit(LP_PREEMPT_REQUEST, &this_lp->bits);
	return (1);
}