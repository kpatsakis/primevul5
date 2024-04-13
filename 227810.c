BOOL rdp_recv_confirm_active(rdpRdp* rdp, wStream* s, UINT16 pduLength)
{
	rdpSettings* settings;
	UINT16 lengthSourceDescriptor;
	UINT16 lengthCombinedCapabilities;
	UINT16 numberCapabilities;
	settings = rdp->settings;

	if (Stream_GetRemainingLength(s) < 10)
		return FALSE;

	Stream_Seek_UINT32(s);                             /* shareId (4 bytes) */
	Stream_Seek_UINT16(s);                             /* originatorId (2 bytes) */
	Stream_Read_UINT16(s, lengthSourceDescriptor);     /* lengthSourceDescriptor (2 bytes) */
	Stream_Read_UINT16(s, lengthCombinedCapabilities); /* lengthCombinedCapabilities (2 bytes) */

	if (Stream_GetRemainingLength(s) < lengthSourceDescriptor + 4U)
		return FALSE;

	Stream_Seek(s, lengthSourceDescriptor);    /* sourceDescriptor */
	Stream_Read_UINT16(s, numberCapabilities); /* numberCapabilities (2 bytes) */
	Stream_Seek(s, 2);                         /* pad2Octets (2 bytes) */
	if (!rdp_read_capability_sets(s, rdp->settings, numberCapabilities, lengthCombinedCapabilities))
		return FALSE;

	if (!settings->ReceivedCapabilities[CAPSET_TYPE_SURFACE_COMMANDS])
	{
		/* client does not support surface commands */
		settings->SurfaceCommandsEnabled = FALSE;
		settings->SurfaceFrameMarkerEnabled = FALSE;
	}

	if (!settings->ReceivedCapabilities[CAPSET_TYPE_FRAME_ACKNOWLEDGE])
	{
		/* client does not support frame acks */
		settings->FrameAcknowledge = 0;
	}

	if (!settings->ReceivedCapabilities[CAPSET_TYPE_BITMAP_CACHE_V3_CODEC_ID])
	{
		/* client does not support bitmap cache v3 */
		settings->BitmapCacheV3Enabled = FALSE;
	}

	if (!settings->ReceivedCapabilities[CAPSET_TYPE_BITMAP_CODECS])
	{
		/* client does not support bitmap codecs */
		settings->RemoteFxCodec = FALSE;
		settings->NSCodec = FALSE;
		settings->JpegCodec = FALSE;
	}

	if (!settings->ReceivedCapabilities[CAPSET_TYPE_MULTI_FRAGMENT_UPDATE])
	{
		/* client does not support multi fragment updates - make sure packages are not fragmented */
		settings->MultifragMaxRequestSize = FASTPATH_FRAGMENT_SAFE_SIZE;
	}

	if (!settings->ReceivedCapabilities[CAPSET_TYPE_LARGE_POINTER])
	{
		/* client does not support large pointers */
		settings->LargePointerFlag = 0;
	}

	return tpkt_ensure_stream_consumed(s, pduLength);
}