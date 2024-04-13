mwifiex_cmd_append_generic_ie(struct mwifiex_private *priv, u8 **buffer)
{
	int ret_len = 0;
	struct mwifiex_ie_types_header ie_header;

	/* Null Checks */
	if (!buffer)
		return 0;
	if (!(*buffer))
		return 0;

	/*
	 * If there is a generic ie buffer setup, append it to the return
	 *   parameter buffer pointer.
	 */
	if (priv->gen_ie_buf_len) {
		mwifiex_dbg(priv->adapter, INFO,
			    "info: %s: append generic ie len %d to %p\n",
			    __func__, priv->gen_ie_buf_len, *buffer);

		/* Wrap the generic IE buffer with a pass through TLV type */
		ie_header.type = cpu_to_le16(TLV_TYPE_PASSTHROUGH);
		ie_header.len = cpu_to_le16(priv->gen_ie_buf_len);
		memcpy(*buffer, &ie_header, sizeof(ie_header));

		/* Increment the return size and the return buffer pointer
		   param */
		*buffer += sizeof(ie_header);
		ret_len += sizeof(ie_header);

		/* Copy the generic IE buffer to the output buffer, advance
		   pointer */
		memcpy(*buffer, priv->gen_ie_buf, priv->gen_ie_buf_len);

		/* Increment the return size and the return buffer pointer
		   param */
		*buffer += priv->gen_ie_buf_len;
		ret_len += priv->gen_ie_buf_len;

		/* Reset the generic IE buffer */
		priv->gen_ie_buf_len = 0;
	}

	/* return the length appended to the buffer */
	return ret_len;
}