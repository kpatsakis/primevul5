static int lp_reset(int minor)
{
	int retval;
	lp_claim_parport_or_block (&lp_table[minor]);
	w_ctr(minor, LP_PSELECP);
	udelay (LP_DELAY);
	w_ctr(minor, LP_PSELECP | LP_PINITP);
	retval = r_str(minor);
	lp_release_parport (&lp_table[minor]);
	return retval;
}