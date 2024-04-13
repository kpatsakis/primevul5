static void lp_release_parport(struct lp_struct *this_lp)
{
	if (test_and_clear_bit(LP_PARPORT_CLAIMED, &this_lp->bits)) {
		parport_release (this_lp->dev);
	}
}