static int ca8210_spi_exchange(
	const u8 *buf,
	size_t len,
	u8 *response,
	void *device_ref
)
{
	int status = 0;
	struct spi_device *spi = device_ref;
	struct ca8210_priv *priv = spi->dev.driver_data;
	long wait_remaining;

	if ((buf[0] & SPI_SYN) && response) { /* if sync wait for confirm */
		reinit_completion(&priv->sync_exchange_complete);
		priv->sync_command_response = response;
	}

	do {
		reinit_completion(&priv->spi_transfer_complete);
		status = ca8210_spi_transfer(priv->spi, buf, len);
		if (status) {
			dev_warn(
				&spi->dev,
				"spi write failed, returned %d\n",
				status
			);
			if (status == -EBUSY)
				continue;
			if (((buf[0] & SPI_SYN) && response))
				complete(&priv->sync_exchange_complete);
			goto cleanup;
		}

		wait_remaining = wait_for_completion_interruptible_timeout(
			&priv->spi_transfer_complete,
			msecs_to_jiffies(1000)
		);
		if (wait_remaining == -ERESTARTSYS) {
			status = -ERESTARTSYS;
		} else if (wait_remaining == 0) {
			dev_err(
				&spi->dev,
				"SPI downstream transfer timed out!\n"
			);
			status = -ETIME;
			goto cleanup;
		}
	} while (status < 0);

	if (!((buf[0] & SPI_SYN) && response))
		goto cleanup;

	wait_remaining = wait_for_completion_interruptible_timeout(
		&priv->sync_exchange_complete,
		msecs_to_jiffies(CA8210_SYNC_TIMEOUT)
	);
	if (wait_remaining == -ERESTARTSYS) {
		status = -ERESTARTSYS;
	} else if (wait_remaining == 0) {
		dev_err(
			&spi->dev,
			"Synchronous confirm timeout\n"
		);
		status = -ETIME;
	}

cleanup:
	priv->sync_command_response = NULL;
	return status;
}