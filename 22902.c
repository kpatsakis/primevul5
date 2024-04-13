static int bos_desc(struct usb_composite_dev *cdev)
{
	struct usb_ext_cap_descriptor	*usb_ext;
	struct usb_dcd_config_params	dcd_config_params;
	struct usb_bos_descriptor	*bos = cdev->req->buf;
	unsigned int			besl = 0;

	bos->bLength = USB_DT_BOS_SIZE;
	bos->bDescriptorType = USB_DT_BOS;

	bos->wTotalLength = cpu_to_le16(USB_DT_BOS_SIZE);
	bos->bNumDeviceCaps = 0;

	/* Get Controller configuration */
	if (cdev->gadget->ops->get_config_params) {
		cdev->gadget->ops->get_config_params(cdev->gadget,
						     &dcd_config_params);
	} else {
		dcd_config_params.besl_baseline =
			USB_DEFAULT_BESL_UNSPECIFIED;
		dcd_config_params.besl_deep =
			USB_DEFAULT_BESL_UNSPECIFIED;
		dcd_config_params.bU1devExitLat =
			USB_DEFAULT_U1_DEV_EXIT_LAT;
		dcd_config_params.bU2DevExitLat =
			cpu_to_le16(USB_DEFAULT_U2_DEV_EXIT_LAT);
	}

	if (dcd_config_params.besl_baseline != USB_DEFAULT_BESL_UNSPECIFIED)
		besl = USB_BESL_BASELINE_VALID |
			USB_SET_BESL_BASELINE(dcd_config_params.besl_baseline);

	if (dcd_config_params.besl_deep != USB_DEFAULT_BESL_UNSPECIFIED)
		besl |= USB_BESL_DEEP_VALID |
			USB_SET_BESL_DEEP(dcd_config_params.besl_deep);

	/*
	 * A SuperSpeed device shall include the USB2.0 extension descriptor
	 * and shall support LPM when operating in USB2.0 HS mode.
	 */
	usb_ext = cdev->req->buf + le16_to_cpu(bos->wTotalLength);
	bos->bNumDeviceCaps++;
	le16_add_cpu(&bos->wTotalLength, USB_DT_USB_EXT_CAP_SIZE);
	usb_ext->bLength = USB_DT_USB_EXT_CAP_SIZE;
	usb_ext->bDescriptorType = USB_DT_DEVICE_CAPABILITY;
	usb_ext->bDevCapabilityType = USB_CAP_TYPE_EXT;
	usb_ext->bmAttributes = cpu_to_le32(USB_LPM_SUPPORT |
					    USB_BESL_SUPPORT | besl);

	/*
	 * The Superspeed USB Capability descriptor shall be implemented by all
	 * SuperSpeed devices.
	 */
	if (gadget_is_superspeed(cdev->gadget)) {
		struct usb_ss_cap_descriptor *ss_cap;

		ss_cap = cdev->req->buf + le16_to_cpu(bos->wTotalLength);
		bos->bNumDeviceCaps++;
		le16_add_cpu(&bos->wTotalLength, USB_DT_USB_SS_CAP_SIZE);
		ss_cap->bLength = USB_DT_USB_SS_CAP_SIZE;
		ss_cap->bDescriptorType = USB_DT_DEVICE_CAPABILITY;
		ss_cap->bDevCapabilityType = USB_SS_CAP_TYPE;
		ss_cap->bmAttributes = 0; /* LTM is not supported yet */
		ss_cap->wSpeedSupported = cpu_to_le16(USB_LOW_SPEED_OPERATION |
						      USB_FULL_SPEED_OPERATION |
						      USB_HIGH_SPEED_OPERATION |
						      USB_5GBPS_OPERATION);
		ss_cap->bFunctionalitySupport = USB_LOW_SPEED_OPERATION;
		ss_cap->bU1devExitLat = dcd_config_params.bU1devExitLat;
		ss_cap->bU2DevExitLat = dcd_config_params.bU2DevExitLat;
	}

	/* The SuperSpeedPlus USB Device Capability descriptor */
	if (gadget_is_superspeed_plus(cdev->gadget)) {
		struct usb_ssp_cap_descriptor *ssp_cap;
		u8 ssac = 1;
		u8 ssic;
		int i;

		if (cdev->gadget->max_ssp_rate == USB_SSP_GEN_2x2)
			ssac = 3;

		/*
		 * Paired RX and TX sublink speed attributes share
		 * the same SSID.
		 */
		ssic = (ssac + 1) / 2 - 1;

		ssp_cap = cdev->req->buf + le16_to_cpu(bos->wTotalLength);
		bos->bNumDeviceCaps++;

		le16_add_cpu(&bos->wTotalLength, USB_DT_USB_SSP_CAP_SIZE(ssac));
		ssp_cap->bLength = USB_DT_USB_SSP_CAP_SIZE(ssac);
		ssp_cap->bDescriptorType = USB_DT_DEVICE_CAPABILITY;
		ssp_cap->bDevCapabilityType = USB_SSP_CAP_TYPE;
		ssp_cap->bReserved = 0;
		ssp_cap->wReserved = 0;

		ssp_cap->bmAttributes =
			cpu_to_le32(FIELD_PREP(USB_SSP_SUBLINK_SPEED_ATTRIBS, ssac) |
				    FIELD_PREP(USB_SSP_SUBLINK_SPEED_IDS, ssic));

		ssp_cap->wFunctionalitySupport =
			cpu_to_le16(FIELD_PREP(USB_SSP_MIN_SUBLINK_SPEED_ATTRIBUTE_ID, 0) |
				    FIELD_PREP(USB_SSP_MIN_RX_LANE_COUNT, 1) |
				    FIELD_PREP(USB_SSP_MIN_TX_LANE_COUNT, 1));

		/*
		 * Use 1 SSID if the gadget supports up to gen2x1 or not
		 * specified:
		 * - SSID 0 for symmetric RX/TX sublink speed of 10 Gbps.
		 *
		 * Use 1 SSID if the gadget supports up to gen1x2:
		 * - SSID 0 for symmetric RX/TX sublink speed of 5 Gbps.
		 *
		 * Use 2 SSIDs if the gadget supports up to gen2x2:
		 * - SSID 0 for symmetric RX/TX sublink speed of 5 Gbps.
		 * - SSID 1 for symmetric RX/TX sublink speed of 10 Gbps.
		 */
		for (i = 0; i < ssac + 1; i++) {
			u8 ssid;
			u8 mantissa;
			u8 type;

			ssid = i >> 1;

			if (cdev->gadget->max_ssp_rate == USB_SSP_GEN_2x1 ||
			    cdev->gadget->max_ssp_rate == USB_SSP_GEN_UNKNOWN)
				mantissa = 10;
			else
				mantissa = 5 << ssid;

			if (i % 2)
				type = USB_SSP_SUBLINK_SPEED_ST_SYM_TX;
			else
				type = USB_SSP_SUBLINK_SPEED_ST_SYM_RX;

			ssp_cap->bmSublinkSpeedAttr[i] =
				cpu_to_le32(FIELD_PREP(USB_SSP_SUBLINK_SPEED_SSID, ssid) |
					    FIELD_PREP(USB_SSP_SUBLINK_SPEED_LSE,
						       USB_SSP_SUBLINK_SPEED_LSE_GBPS) |
					    FIELD_PREP(USB_SSP_SUBLINK_SPEED_ST, type) |
					    FIELD_PREP(USB_SSP_SUBLINK_SPEED_LP,
						       USB_SSP_SUBLINK_SPEED_LP_SSP) |
					    FIELD_PREP(USB_SSP_SUBLINK_SPEED_LSM, mantissa));
		}
	}

	return le16_to_cpu(bos->wTotalLength);
}