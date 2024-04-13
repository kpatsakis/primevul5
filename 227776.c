BOOL rdp_recv_get_active_header(rdpRdp* rdp, wStream* s, UINT16* pChannelId, UINT16* length)
{
	UINT16 securityFlags = 0;

	if (!rdp_read_header(rdp, s, length, pChannelId))
		return FALSE;

	if (freerdp_shall_disconnect(rdp->instance))
		return TRUE;

	if (rdp->settings->UseRdpSecurityLayer)
	{
		if (!rdp_read_security_header(s, &securityFlags, length))
			return FALSE;

		if (securityFlags & SEC_ENCRYPT)
		{
			if (!rdp_decrypt(rdp, s, length, securityFlags))
			{
				WLog_ERR(TAG, "rdp_decrypt failed");
				return FALSE;
			}
		}
	}

	if (*pChannelId != MCS_GLOBAL_CHANNEL_ID)
	{
		UINT16 mcsMessageChannelId = rdp->mcs->messageChannelId;

		if ((mcsMessageChannelId == 0) || (*pChannelId != mcsMessageChannelId))
		{
			WLog_ERR(TAG, "unexpected MCS channel id %04" PRIx16 " received", *pChannelId);
			return FALSE;
		}
	}

	return TRUE;
}