int license_recv(rdpLicense* license, wStream* s)
{
	BYTE flags;
	BYTE bMsgType;
	UINT16 wMsgSize;
	UINT16 length;
	UINT16 channelId;
	UINT16 securityFlags = 0;

	if (!rdp_read_header(license->rdp, s, &length, &channelId))
	{
		WLog_ERR(TAG, "Incorrect RDP header.");
		return -1;
	}

	if (!rdp_read_security_header(s, &securityFlags, &length))
		return -1;

	if (securityFlags & SEC_ENCRYPT)
	{
		if (!rdp_decrypt(license->rdp, s, &length, securityFlags))
		{
			WLog_ERR(TAG, "rdp_decrypt failed");
			return -1;
		}
	}

	if (!(securityFlags & SEC_LICENSE_PKT))
	{
		int status;

		if (!(securityFlags & SEC_ENCRYPT))
			Stream_Rewind(s, RDP_SECURITY_HEADER_LENGTH);

		status = rdp_recv_out_of_sequence_pdu(license->rdp, s);
		if (status < 0)
		{
			WLog_ERR(TAG, "unexpected license packet.");
			return status;
		}

		return 0;
	}

	if (!license_read_preamble(s, &bMsgType, &flags, &wMsgSize)) /* preamble (4 bytes) */
		return -1;

	DEBUG_LICENSE("Receiving %s Packet", LICENSE_MESSAGE_STRINGS[bMsgType & 0x1F]);

	switch (bMsgType)
	{
		case LICENSE_REQUEST:
			if (!license_read_license_request_packet(license, s))
				return -1;

			if (!license_answer_license_request(license))
				return -1;
			break;

		case PLATFORM_CHALLENGE:
			if (!license_read_platform_challenge_packet(license, s))
				return -1;

			if (!license_send_platform_challenge_response_packet(license))
				return -1;
			break;

		case NEW_LICENSE:
		case UPGRADE_LICENSE:
			if (!license_read_new_or_upgrade_license_packet(license, s))
				return -1;
			break;

		case ERROR_ALERT:
			if (!license_read_error_alert_packet(license, s))
				return -1;
			break;

		default:
			WLog_ERR(TAG, "invalid bMsgType:%" PRIu8 "", bMsgType);
			return -1;
	}

	if (!tpkt_ensure_stream_consumed(s, length))
		return -1;
	return 0;
}