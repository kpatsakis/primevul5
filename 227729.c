static int uvc_scan_device(struct uvc_device *dev)
{
	struct uvc_video_chain *chain;
	struct uvc_entity *term;

	list_for_each_entry(term, &dev->entities, list) {
		if (!UVC_ENTITY_IS_OTERM(term))
			continue;

		/* If the terminal is already included in a chain, skip it.
		 * This can happen for chains that have multiple output
		 * terminals, where all output terminals beside the first one
		 * will be inserted in the chain in forward scans.
		 */
		if (term->chain.next || term->chain.prev)
			continue;

		chain = uvc_alloc_chain(dev);
		if (chain == NULL)
			return -ENOMEM;

		term->flags |= UVC_ENTITY_FLAG_DEFAULT;

		if (uvc_scan_chain(chain, term) < 0) {
			kfree(chain);
			continue;
		}

		uvc_trace(UVC_TRACE_PROBE, "Found a valid video chain (%s).\n",
			  uvc_print_chain(chain));

		list_add_tail(&chain->list, &dev->chains);
	}

	if (list_empty(&dev->chains))
		uvc_scan_fallback(dev);

	if (list_empty(&dev->chains)) {
		uvc_printk(KERN_INFO, "No valid video chain found.\n");
		return -1;
	}

	return 0;
}