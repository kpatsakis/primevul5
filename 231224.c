mwifiex_cmd_append_wps_ie(struct mwifiex_private *priv, u8 **buffer)
{
	int retLen = 0;
	struct mwifiex_ie_types_header ie_header;

	if (!buffer || !*buffer)
		return 0;

	/*
	 * If there is a wps ie buffer setup, append it to the return
	 * parameter buffer pointer.
	 */
	if (priv->wps_ie_len) {
		mwifiex_dbg(priv->adapter, CMD,
			    "cmd: append wps ie %d to %p\n",
			    priv->wps_ie_len, *buffer);

		/* Wrap the generic IE buffer with a pass through TLV type */
		ie_header.type = cpu_to_le16(TLV_TYPE_PASSTHROUGH);
		ie_header.len = cpu_to_le16(priv->wps_ie_len);
		memcpy(*buffer, &ie_header, sizeof(ie_header));
		*buffer += sizeof(ie_header);
		retLen += sizeof(ie_header);

		memcpy(*buffer, priv->wps_ie, priv->wps_ie_len);
		*buffer += priv->wps_ie_len;
		retLen += priv->wps_ie_len;

	}

	kfree(priv->wps_ie);
	priv->wps_ie_len = 0;
	return retLen;
}