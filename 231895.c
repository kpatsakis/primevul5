static int lp_release(struct inode * inode, struct file * file)
{
	unsigned int minor = iminor(inode);

	lp_claim_parport_or_block (&lp_table[minor]);
	parport_negotiate (lp_table[minor].dev->port, IEEE1284_MODE_COMPAT);
	lp_table[minor].current_mode = IEEE1284_MODE_COMPAT;
	lp_release_parport (&lp_table[minor]);
	kfree(lp_table[minor].lp_buffer);
	lp_table[minor].lp_buffer = NULL;
	LP_F(minor) &= ~LP_BUSY;
	return 0;
}