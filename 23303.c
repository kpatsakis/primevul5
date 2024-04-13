static int write_queue_xenstore_keys(struct netfront_queue *queue,
			   struct xenbus_transaction *xbt, int write_hierarchical)
{
	/* Write the queue-specific keys into XenStore in the traditional
	 * way for a single queue, or in a queue subkeys for multiple
	 * queues.
	 */
	struct xenbus_device *dev = queue->info->xbdev;
	int err;
	const char *message;
	char *path;
	size_t pathsize;

	/* Choose the correct place to write the keys */
	if (write_hierarchical) {
		pathsize = strlen(dev->nodename) + 10;
		path = kzalloc(pathsize, GFP_KERNEL);
		if (!path) {
			err = -ENOMEM;
			message = "out of memory while writing ring references";
			goto error;
		}
		snprintf(path, pathsize, "%s/queue-%u",
				dev->nodename, queue->id);
	} else {
		path = (char *)dev->nodename;
	}

	/* Write ring references */
	err = xenbus_printf(*xbt, path, "tx-ring-ref", "%u",
			queue->tx_ring_ref);
	if (err) {
		message = "writing tx-ring-ref";
		goto error;
	}

	err = xenbus_printf(*xbt, path, "rx-ring-ref", "%u",
			queue->rx_ring_ref);
	if (err) {
		message = "writing rx-ring-ref";
		goto error;
	}

	/* Write event channels; taking into account both shared
	 * and split event channel scenarios.
	 */
	if (queue->tx_evtchn == queue->rx_evtchn) {
		/* Shared event channel */
		err = xenbus_printf(*xbt, path,
				"event-channel", "%u", queue->tx_evtchn);
		if (err) {
			message = "writing event-channel";
			goto error;
		}
	} else {
		/* Split event channels */
		err = xenbus_printf(*xbt, path,
				"event-channel-tx", "%u", queue->tx_evtchn);
		if (err) {
			message = "writing event-channel-tx";
			goto error;
		}

		err = xenbus_printf(*xbt, path,
				"event-channel-rx", "%u", queue->rx_evtchn);
		if (err) {
			message = "writing event-channel-rx";
			goto error;
		}
	}

	if (write_hierarchical)
		kfree(path);
	return 0;

error:
	if (write_hierarchical)
		kfree(path);
	xenbus_dev_fatal(dev, err, "%s", message);
	return err;
}