static void xennet_bus_close(struct xenbus_device *dev)
{
	int ret;

	if (xenbus_read_driver_state(dev->otherend) == XenbusStateClosed)
		return;
	do {
		xenbus_switch_state(dev, XenbusStateClosing);
		ret = wait_event_timeout(module_wq,
				   xenbus_read_driver_state(dev->otherend) ==
				   XenbusStateClosing ||
				   xenbus_read_driver_state(dev->otherend) ==
				   XenbusStateClosed ||
				   xenbus_read_driver_state(dev->otherend) ==
				   XenbusStateUnknown,
				   XENNET_TIMEOUT);
	} while (!ret);

	if (xenbus_read_driver_state(dev->otherend) == XenbusStateClosed)
		return;

	do {
		xenbus_switch_state(dev, XenbusStateClosed);
		ret = wait_event_timeout(module_wq,
				   xenbus_read_driver_state(dev->otherend) ==
				   XenbusStateClosed ||
				   xenbus_read_driver_state(dev->otherend) ==
				   XenbusStateUnknown,
				   XENNET_TIMEOUT);
	} while (!ret);
}