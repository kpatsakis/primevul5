static int uvc_scan_chain_entity(struct uvc_video_chain *chain,
	struct uvc_entity *entity)
{
	switch (UVC_ENTITY_TYPE(entity)) {
	case UVC_VC_EXTENSION_UNIT:
		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(KERN_CONT " <- XU %d", entity->id);

		if (entity->bNrInPins != 1) {
			uvc_trace(UVC_TRACE_DESCR, "Extension unit %d has more "
				"than 1 input pin.\n", entity->id);
			return -1;
		}

		break;

	case UVC_VC_PROCESSING_UNIT:
		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(KERN_CONT " <- PU %d", entity->id);

		if (chain->processing != NULL) {
			uvc_trace(UVC_TRACE_DESCR, "Found multiple "
				"Processing Units in chain.\n");
			return -1;
		}

		chain->processing = entity;
		break;

	case UVC_VC_SELECTOR_UNIT:
		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(KERN_CONT " <- SU %d", entity->id);

		/* Single-input selector units are ignored. */
		if (entity->bNrInPins == 1)
			break;

		if (chain->selector != NULL) {
			uvc_trace(UVC_TRACE_DESCR, "Found multiple Selector "
				"Units in chain.\n");
			return -1;
		}

		chain->selector = entity;
		break;

	case UVC_ITT_VENDOR_SPECIFIC:
	case UVC_ITT_CAMERA:
	case UVC_ITT_MEDIA_TRANSPORT_INPUT:
		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(KERN_CONT " <- IT %d\n", entity->id);

		break;

	case UVC_OTT_VENDOR_SPECIFIC:
	case UVC_OTT_DISPLAY:
	case UVC_OTT_MEDIA_TRANSPORT_OUTPUT:
		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(KERN_CONT " OT %d", entity->id);

		break;

	case UVC_TT_STREAMING:
		if (UVC_ENTITY_IS_ITERM(entity)) {
			if (uvc_trace_param & UVC_TRACE_PROBE)
				printk(KERN_CONT " <- IT %d\n", entity->id);
		} else {
			if (uvc_trace_param & UVC_TRACE_PROBE)
				printk(KERN_CONT " OT %d", entity->id);
		}

		break;

	default:
		uvc_trace(UVC_TRACE_DESCR, "Unsupported entity type "
			"0x%04x found in chain.\n", UVC_ENTITY_TYPE(entity));
		return -1;
	}

	list_add_tail(&entity->chain, &chain->entities);
	return 0;
}