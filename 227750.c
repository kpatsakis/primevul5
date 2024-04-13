static BOOL rdp_read_bitmap_codecs_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	BYTE codecId;
	GUID codecGuid;
	RPC_STATUS rpc_status;
	BYTE bitmapCodecCount;
	UINT16 codecPropertiesLength;
	UINT16 remainingLength;
	BOOL guidNSCodec = FALSE;
	BOOL guidRemoteFx = FALSE;
	BOOL guidRemoteFxImage = FALSE;

	if (length < 5)
		return FALSE;

	Stream_Read_UINT8(s, bitmapCodecCount); /* bitmapCodecCount (1 byte) */
	remainingLength = length - 5;

	while (bitmapCodecCount > 0)
	{
		if (remainingLength < 19)
			return FALSE;

		rdp_read_bitmap_codec_guid(s, &codecGuid);    /* codecGuid (16 bytes) */
		Stream_Read_UINT8(s, codecId);                /* codecId (1 byte) */
		Stream_Read_UINT16(s, codecPropertiesLength); /* codecPropertiesLength (2 bytes) */
		remainingLength -= 19;

		if (remainingLength < codecPropertiesLength)
			return FALSE;

		if (settings->ServerMode)
		{
			UINT32 beg;
			UINT32 end;
			beg = (UINT32)Stream_GetPosition(s);
			end = beg + codecPropertiesLength;

			if (UuidEqual(&codecGuid, &CODEC_GUID_REMOTEFX, &rpc_status))
			{
				UINT32 rfxCapsLength;
				UINT32 rfxPropsLength;
				UINT32 captureFlags;
				guidRemoteFx = TRUE;
				settings->RemoteFxCodecId = codecId;
				Stream_Read_UINT32(s, rfxPropsLength); /* length (4 bytes) */
				Stream_Read_UINT32(s, captureFlags);   /* captureFlags (4 bytes) */
				Stream_Read_UINT32(s, rfxCapsLength);  /* capsLength (4 bytes) */
				settings->RemoteFxCaptureFlags = captureFlags;
				settings->RemoteFxOnly = (captureFlags & CARDP_CAPS_CAPTURE_NON_CAC) ? TRUE : FALSE;

				if (rfxCapsLength)
				{
					UINT16 blockType;
					UINT32 blockLen;
					UINT16 numCapsets;
					BYTE rfxCodecId;
					UINT16 capsetType;
					UINT16 numIcaps;
					UINT16 icapLen;
					/* TS_RFX_CAPS */
					Stream_Read_UINT16(s, blockType);  /* blockType (2 bytes) */
					Stream_Read_UINT32(s, blockLen);   /* blockLen (4 bytes) */
					Stream_Read_UINT16(s, numCapsets); /* numCapsets (2 bytes) */

					if (blockType != 0xCBC0)
						return FALSE;

					if (blockLen != 8)
						return FALSE;

					if (numCapsets != 1)
						return FALSE;

					/* TS_RFX_CAPSET */
					Stream_Read_UINT16(s, blockType);  /* blockType (2 bytes) */
					Stream_Read_UINT32(s, blockLen);   /* blockLen (4 bytes) */
					Stream_Read_UINT8(s, rfxCodecId);  /* codecId (1 byte) */
					Stream_Read_UINT16(s, capsetType); /* capsetType (2 bytes) */
					Stream_Read_UINT16(s, numIcaps);   /* numIcaps (2 bytes) */
					Stream_Read_UINT16(s, icapLen);    /* icapLen (2 bytes) */

					if (blockType != 0xCBC1)
						return FALSE;

					if (rfxCodecId != 1)
						return FALSE;

					if (capsetType != 0xCFC0)
						return FALSE;

					while (numIcaps--)
					{
						UINT16 version;
						UINT16 tileSize;
						BYTE codecFlags;
						BYTE colConvBits;
						BYTE transformBits;
						BYTE entropyBits;
						/* TS_RFX_ICAP */
						Stream_Read_UINT16(s, version);      /* version (2 bytes) */
						Stream_Read_UINT16(s, tileSize);     /* tileSize (2 bytes) */
						Stream_Read_UINT8(s, codecFlags);    /* flags (1 byte) */
						Stream_Read_UINT8(s, colConvBits);   /* colConvBits (1 byte) */
						Stream_Read_UINT8(s, transformBits); /* transformBits (1 byte) */
						Stream_Read_UINT8(s, entropyBits);   /* entropyBits (1 byte) */

						if (version == 0x0009)
						{
							/* Version 0.9 */
							if (tileSize != 0x0080)
								return FALSE;
						}
						else if (version == 0x0100)
						{
							/* Version 1.0 */
							if (tileSize != 0x0040)
								return FALSE;
						}
						else
							return FALSE;

						if (colConvBits != 1)
							return FALSE;

						if (transformBits != 1)
							return FALSE;
					}
				}
			}
			else if (UuidEqual(&codecGuid, &CODEC_GUID_IMAGE_REMOTEFX, &rpc_status))
			{
				/* Microsoft RDP servers ignore CODEC_GUID_IMAGE_REMOTEFX codec properties */
				guidRemoteFxImage = TRUE;
				Stream_Seek(s, codecPropertiesLength); /* codecProperties */
			}
			else if (UuidEqual(&codecGuid, &CODEC_GUID_NSCODEC, &rpc_status))
			{
				BYTE colorLossLevel;
				BYTE fAllowSubsampling;
				BYTE fAllowDynamicFidelity;
				guidNSCodec = TRUE;
				settings->NSCodecId = codecId;
				Stream_Read_UINT8(s, fAllowDynamicFidelity); /* fAllowDynamicFidelity (1 byte) */
				Stream_Read_UINT8(s, fAllowSubsampling);     /* fAllowSubsampling (1 byte) */
				Stream_Read_UINT8(s, colorLossLevel);        /* colorLossLevel (1 byte) */

				if (colorLossLevel < 1)
					colorLossLevel = 1;

				if (colorLossLevel > 7)
					colorLossLevel = 7;

				settings->NSCodecAllowDynamicColorFidelity = fAllowDynamicFidelity;
				settings->NSCodecAllowSubsampling = fAllowSubsampling;
				settings->NSCodecColorLossLevel = colorLossLevel;
			}
			else if (UuidEqual(&codecGuid, &CODEC_GUID_IGNORE, &rpc_status))
			{
				Stream_Seek(s, codecPropertiesLength); /* codecProperties */
			}
			else
			{
				Stream_Seek(s, codecPropertiesLength); /* codecProperties */
			}

			if (Stream_GetPosition(s) != end)
			{
				WLog_ERR(TAG,
				         "error while reading codec properties: actual offset: %" PRIuz
				         " expected offset: %" PRIu32 "",
				         Stream_GetPosition(s), end);
				Stream_SetPosition(s, end);
			}

			remainingLength -= codecPropertiesLength;
		}
		else
		{
			Stream_Seek(s, codecPropertiesLength); /* codecProperties */
			remainingLength -= codecPropertiesLength;
		}

		bitmapCodecCount--;
	}

	if (settings->ServerMode)
	{
		/* only enable a codec if we've announced/enabled it before */
		settings->RemoteFxCodec = settings->RemoteFxCodec && guidRemoteFx;
		settings->RemoteFxImageCodec = settings->RemoteFxImageCodec && guidRemoteFxImage;
		settings->NSCodec = settings->NSCodec && guidNSCodec;
		settings->JpegCodec = FALSE;
	}

	return TRUE;
}