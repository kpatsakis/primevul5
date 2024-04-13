static BOOL rdp_write_bitmap_codecs_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	BYTE bitmapCodecCount;

	if (!Stream_EnsureRemainingCapacity(s, 64))
		return FALSE;

	header = rdp_capability_set_start(s);
	bitmapCodecCount = 0;

	if (settings->RemoteFxCodec)
		bitmapCodecCount++;

	if (settings->NSCodec)
		bitmapCodecCount++;

#if defined(WITH_JPEG)

	if (settings->JpegCodec)
		bitmapCodecCount++;

#endif

	if (settings->RemoteFxImageCodec)
		bitmapCodecCount++;

	Stream_Write_UINT8(s, bitmapCodecCount);

	if (settings->RemoteFxCodec)
	{
		rdp_write_bitmap_codec_guid(s, &CODEC_GUID_REMOTEFX); /* codecGUID */

		if (settings->ServerMode)
		{
			Stream_Write_UINT8(s, 0); /* codecID is defined by the client */

			if (!rdp_write_rfx_server_capability_container(s, settings))
				return FALSE;
		}
		else
		{
			Stream_Write_UINT8(s, RDP_CODEC_ID_REMOTEFX); /* codecID */

			if (!rdp_write_rfx_client_capability_container(s, settings))
				return FALSE;
		}
	}

	if (settings->NSCodec)
	{
		rdp_write_bitmap_codec_guid(s, &CODEC_GUID_NSCODEC); /* codecGUID */

		if (settings->ServerMode)
		{
			Stream_Write_UINT8(s, 0); /* codecID is defined by the client */

			if (!rdp_write_nsc_server_capability_container(s, settings))
				return FALSE;
		}
		else
		{
			Stream_Write_UINT8(s, RDP_CODEC_ID_NSCODEC); /* codecID */

			if (!rdp_write_nsc_client_capability_container(s, settings))
				return FALSE;
		}
	}

#if defined(WITH_JPEG)

	if (settings->JpegCodec)
	{
		rdp_write_bitmap_codec_guid(s, &CODEC_GUID_JPEG); /* codecGUID */

		if (settings->ServerMode)
		{
			Stream_Write_UINT8(s, 0); /* codecID is defined by the client */

			if (!rdp_write_jpeg_server_capability_container(s, settings))
				return FALSE;
		}
		else
		{
			Stream_Write_UINT8(s, RDP_CODEC_ID_JPEG); /* codecID */

			if (!rdp_write_jpeg_client_capability_container(s, settings))
				return FALSE;
		}
	}

#endif

	if (settings->RemoteFxImageCodec)
	{
		rdp_write_bitmap_codec_guid(s, &CODEC_GUID_IMAGE_REMOTEFX); /* codecGUID */

		if (settings->ServerMode)
		{
			Stream_Write_UINT8(s, 0); /* codecID is defined by the client */

			if (!rdp_write_rfx_server_capability_container(s, settings))
				return FALSE;
		}
		else
		{
			Stream_Write_UINT8(s, RDP_CODEC_ID_IMAGE_REMOTEFX); /* codecID */

			if (!rdp_write_rfx_client_capability_container(s, settings))
				return FALSE;
		}
	}

	rdp_capability_set_finish(s, header, CAPSET_TYPE_BITMAP_CODECS);
	return TRUE;
}