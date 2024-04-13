mwifiex_cmd_append_tsf_tlv(struct mwifiex_private *priv, u8 **buffer,
			   struct mwifiex_bssdescriptor *bss_desc)
{
	struct mwifiex_ie_types_tsf_timestamp tsf_tlv;
	__le64 tsf_val;

	/* Null Checks */
	if (buffer == NULL)
		return 0;
	if (*buffer == NULL)
		return 0;

	memset(&tsf_tlv, 0x00, sizeof(struct mwifiex_ie_types_tsf_timestamp));

	tsf_tlv.header.type = cpu_to_le16(TLV_TYPE_TSFTIMESTAMP);
	tsf_tlv.header.len = cpu_to_le16(2 * sizeof(tsf_val));

	memcpy(*buffer, &tsf_tlv, sizeof(tsf_tlv.header));
	*buffer += sizeof(tsf_tlv.header);

	/* TSF at the time when beacon/probe_response was received */
	tsf_val = cpu_to_le64(bss_desc->fw_tsf);
	memcpy(*buffer, &tsf_val, sizeof(tsf_val));
	*buffer += sizeof(tsf_val);

	tsf_val = cpu_to_le64(bss_desc->timestamp);

	mwifiex_dbg(priv->adapter, INFO,
		    "info: %s: TSF offset calc: %016llx - %016llx\n",
		    __func__, bss_desc->timestamp, bss_desc->fw_tsf);

	memcpy(*buffer, &tsf_val, sizeof(tsf_val));
	*buffer += sizeof(tsf_val);

	return sizeof(tsf_tlv.header) + (2 * sizeof(tsf_val));
}