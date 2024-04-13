static ssize_t ca8210_test_int_user_write(
	struct file        *filp,
	const char __user  *in_buf,
	size_t              len,
	loff_t             *off
)
{
	int ret;
	struct ca8210_priv *priv = filp->private_data;
	u8 command[CA8210_SPI_BUF_SIZE];

	memset(command, SPI_IDLE, 6);
	if (len > CA8210_SPI_BUF_SIZE || len < 2) {
		dev_warn(
			&priv->spi->dev,
			"userspace requested erroneous write length (%zu)\n",
			len
		);
		return -EBADE;
	}

	ret = copy_from_user(command, in_buf, len);
	if (ret) {
		dev_err(
			&priv->spi->dev,
			"%d bytes could not be copied from userspace\n",
			ret
		);
		return -EIO;
	}
	if (len != command[1] + 2) {
		dev_err(
			&priv->spi->dev,
			"write len does not match packet length field\n"
		);
		return -EBADE;
	}

	ret = ca8210_test_check_upstream(command, priv->spi);
	if (ret == 0) {
		ret = ca8210_spi_exchange(
			command,
			command[1] + 2,
			NULL,
			priv->spi
		);
		if (ret < 0) {
			/* effectively 0 bytes were written successfully */
			dev_err(
				&priv->spi->dev,
				"spi exchange failed\n"
			);
			return ret;
		}
		if (command[0] & SPI_SYN)
			priv->sync_down++;
	}

	return len;
}