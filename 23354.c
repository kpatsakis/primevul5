static int xennet_remove(struct xenbus_device *dev)
{
	struct netfront_info *info = dev_get_drvdata(&dev->dev);

	xennet_bus_close(dev);
	xennet_disconnect_backend(info);

	if (info->netdev->reg_state == NETREG_REGISTERED)
		unregister_netdev(info->netdev);

	if (info->queues) {
		rtnl_lock();
		xennet_destroy_queues(info);
		rtnl_unlock();
	}
	xennet_free_netdev(info->netdev);

	return 0;
}