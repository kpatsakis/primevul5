static int ca8210_test_int_open(struct inode *inodp, struct file *filp)
{
	struct ca8210_priv *priv = inodp->i_private;

	filp->private_data = priv;
	return 0;
}