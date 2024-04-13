static long ca8210_test_int_ioctl(
	struct file *filp,
	unsigned int ioctl_num,
	unsigned long ioctl_param
)
{
	struct ca8210_priv *priv = filp->private_data;

	switch (ioctl_num) {
	case CA8210_IOCTL_HARD_RESET:
		ca8210_reset_send(priv->spi, ioctl_param);
		break;
	default:
		break;
	}
	return 0;
}