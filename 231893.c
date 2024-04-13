static void lp_claim_parport_or_block(struct lp_struct *this_lp)
{
	if (!test_and_set_bit(LP_PARPORT_CLAIMED, &this_lp->bits)) {
		parport_claim_or_block (this_lp->dev);
	}
}