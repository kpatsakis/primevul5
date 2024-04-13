static void shutdown_busid(struct bus_id_priv *busid_priv)
{
	usbip_event_add(&busid_priv->sdev->ud, SDEV_EVENT_REMOVED);

	/* wait for the stop of the event handler */
	usbip_stop_eh(&busid_priv->sdev->ud);
}