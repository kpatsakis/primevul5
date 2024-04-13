static int netfront_resume(struct xenbus_device *dev)
{
	struct netfront_info *info = dev_get_drvdata(&dev->dev);

	dev_dbg(&dev->dev, "%s\n", dev->nodename);

	netif_tx_lock_bh(info->netdev);
	netif_device_detach(info->netdev);
	netif_tx_unlock_bh(info->netdev);

	xennet_disconnect_backend(info);
	return 0;
}