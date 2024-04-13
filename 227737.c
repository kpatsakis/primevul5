static int uvc_scan_fallback(struct uvc_device *dev)
{
	struct uvc_video_chain *chain;
	struct uvc_entity *iterm = NULL;
	struct uvc_entity *oterm = NULL;
	struct uvc_entity *entity;
	struct uvc_entity *prev;

	/*
	 * Start by locating the input and output terminals. We only support
	 * devices with exactly one of each for now.
	 */
	list_for_each_entry(entity, &dev->entities, list) {
		if (UVC_ENTITY_IS_ITERM(entity)) {
			if (iterm)
				return -EINVAL;
			iterm = entity;
		}

		if (UVC_ENTITY_IS_OTERM(entity)) {
			if (oterm)
				return -EINVAL;
			oterm = entity;
		}
	}

	if (iterm == NULL || oterm == NULL)
		return -EINVAL;

	/* Allocate the chain and fill it. */
	chain = uvc_alloc_chain(dev);
	if (chain == NULL)
		return -ENOMEM;

	if (uvc_scan_chain_entity(chain, oterm) < 0)
		goto error;

	prev = oterm;

	/*
	 * Add all Processing and Extension Units with two pads. The order
	 * doesn't matter much, use reverse list traversal to connect units in
	 * UVC descriptor order as we build the chain from output to input. This
	 * leads to units appearing in the order meant by the manufacturer for
	 * the cameras known to require this heuristic.
	 */
	list_for_each_entry_reverse(entity, &dev->entities, list) {
		if (entity->type != UVC_VC_PROCESSING_UNIT &&
		    entity->type != UVC_VC_EXTENSION_UNIT)
			continue;

		if (entity->num_pads != 2)
			continue;

		if (uvc_scan_chain_entity(chain, entity) < 0)
			goto error;

		prev->baSourceID[0] = entity->id;
		prev = entity;
	}

	if (uvc_scan_chain_entity(chain, iterm) < 0)
		goto error;

	prev->baSourceID[0] = iterm->id;

	list_add_tail(&chain->list, &dev->chains);

	uvc_trace(UVC_TRACE_PROBE,
		  "Found a video chain by fallback heuristic (%s).\n",
		  uvc_print_chain(chain));

	return 0;

error:
	kfree(chain);
	return -EINVAL;
}