static BOOL rdp_write_rfx_client_capability_container(wStream* s, const rdpSettings* settings)
{
	UINT32 captureFlags;
	BYTE codecMode;

	if (!Stream_EnsureRemainingCapacity(s, 64))
		return FALSE;

	captureFlags = settings->RemoteFxOnly ? 0 : CARDP_CAPS_CAPTURE_NON_CAC;
	codecMode = settings->RemoteFxCodecMode;
	Stream_Write_UINT16(s, 49); /* codecPropertiesLength */
	/* TS_RFX_CLNT_CAPS_CONTAINER */
	Stream_Write_UINT32(s, 49);           /* length */
	Stream_Write_UINT32(s, captureFlags); /* captureFlags */
	Stream_Write_UINT32(s, 37);           /* capsLength */
	/* TS_RFX_CAPS */
	Stream_Write_UINT16(s, CBY_CAPS); /* blockType */
	Stream_Write_UINT32(s, 8);        /* blockLen */
	Stream_Write_UINT16(s, 1);        /* numCapsets */
	/* TS_RFX_CAPSET */
	Stream_Write_UINT16(s, CBY_CAPSET); /* blockType */
	Stream_Write_UINT32(s, 29);         /* blockLen */
	Stream_Write_UINT8(s, 0x01);        /* codecId (MUST be set to 0x01) */
	Stream_Write_UINT16(s, CLY_CAPSET); /* capsetType */
	Stream_Write_UINT16(s, 2);          /* numIcaps */
	Stream_Write_UINT16(s, 8);          /* icapLen */
	/* TS_RFX_ICAP (RLGR1) */
	Stream_Write_UINT16(s, CLW_VERSION_1_0);   /* version */
	Stream_Write_UINT16(s, CT_TILE_64x64);     /* tileSize */
	Stream_Write_UINT8(s, codecMode);          /* flags */
	Stream_Write_UINT8(s, CLW_COL_CONV_ICT);   /* colConvBits */
	Stream_Write_UINT8(s, CLW_XFORM_DWT_53_A); /* transformBits */
	Stream_Write_UINT8(s, CLW_ENTROPY_RLGR1);  /* entropyBits */
	/* TS_RFX_ICAP (RLGR3) */
	Stream_Write_UINT16(s, CLW_VERSION_1_0);   /* version */
	Stream_Write_UINT16(s, CT_TILE_64x64);     /* tileSize */
	Stream_Write_UINT8(s, codecMode);          /* flags */
	Stream_Write_UINT8(s, CLW_COL_CONV_ICT);   /* colConvBits */
	Stream_Write_UINT8(s, CLW_XFORM_DWT_53_A); /* transformBits */
	Stream_Write_UINT8(s, CLW_ENTROPY_RLGR3);  /* entropyBits */
	return TRUE;
}