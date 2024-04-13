static BOOL rdp_write_confirm_active(wStream* s, rdpSettings* settings)
{
	size_t bm, em, lm;
	UINT16 numberCapabilities;
	UINT16 lengthSourceDescriptor;
	size_t lengthCombinedCapabilities;
	BOOL ret;
	lengthSourceDescriptor = sizeof(SOURCE_DESCRIPTOR);
	Stream_Write_UINT32(s, settings->ShareId);      /* shareId (4 bytes) */
	Stream_Write_UINT16(s, 0x03EA);                 /* originatorId (2 bytes) */
	Stream_Write_UINT16(s, lengthSourceDescriptor); /* lengthSourceDescriptor (2 bytes) */
	lm = Stream_GetPosition(s);
	Stream_Seek_UINT16(s); /* lengthCombinedCapabilities (2 bytes) */
	Stream_Write(s, SOURCE_DESCRIPTOR, lengthSourceDescriptor); /* sourceDescriptor */
	bm = Stream_GetPosition(s);
	Stream_Seek_UINT16(s);     /* numberCapabilities (2 bytes) */
	Stream_Write_UINT16(s, 0); /* pad2Octets (2 bytes) */
	/* Capability Sets */
	numberCapabilities = 15;

	if (!rdp_write_general_capability_set(s, settings) ||
	    !rdp_write_bitmap_capability_set(s, settings) ||
	    !rdp_write_order_capability_set(s, settings))
		return FALSE;

	if (settings->RdpVersion >= RDP_VERSION_5_PLUS)
		ret = rdp_write_bitmap_cache_v2_capability_set(s, settings);
	else
		ret = rdp_write_bitmap_cache_capability_set(s, settings);

	if (!ret)
		return FALSE;

	if (!rdp_write_pointer_capability_set(s, settings) ||
	    !rdp_write_input_capability_set(s, settings) ||
	    !rdp_write_brush_capability_set(s, settings) ||
	    !rdp_write_glyph_cache_capability_set(s, settings) ||
	    !rdp_write_virtual_channel_capability_set(s, settings) ||
	    !rdp_write_sound_capability_set(s, settings) ||
	    !rdp_write_share_capability_set(s, settings) ||
	    !rdp_write_font_capability_set(s, settings) ||
	    !rdp_write_control_capability_set(s, settings) ||
	    !rdp_write_color_cache_capability_set(s, settings) ||
	    !rdp_write_window_activation_capability_set(s, settings))
	{
		return FALSE;
	}

	if (settings->OffscreenSupportLevel)
	{
		numberCapabilities++;

		if (!rdp_write_offscreen_bitmap_cache_capability_set(s, settings))
			return FALSE;
	}

	if (settings->DrawNineGridEnabled)
	{
		numberCapabilities++;

		if (!rdp_write_draw_nine_grid_cache_capability_set(s, settings))
			return FALSE;
	}

	if (settings->ReceivedCapabilities[CAPSET_TYPE_LARGE_POINTER])
	{
		if (settings->LargePointerFlag)
		{
			numberCapabilities++;

			if (!rdp_write_large_pointer_capability_set(s, settings))
				return FALSE;
		}
	}

	if (settings->RemoteApplicationMode)
	{
		numberCapabilities += 2;

		if (!rdp_write_remote_programs_capability_set(s, settings) ||
		    !rdp_write_window_list_capability_set(s, settings))
			return FALSE;
	}

	if (settings->ReceivedCapabilities[CAPSET_TYPE_MULTI_FRAGMENT_UPDATE])
	{
		numberCapabilities++;

		if (!rdp_write_multifragment_update_capability_set(s, settings))
			return FALSE;
	}

	if (settings->ReceivedCapabilities[CAPSET_TYPE_SURFACE_COMMANDS])
	{
		numberCapabilities++;

		if (!rdp_write_surface_commands_capability_set(s, settings))
			return FALSE;
	}

	if (settings->ReceivedCapabilities[CAPSET_TYPE_BITMAP_CODECS])
	{
		numberCapabilities++;

		if (!rdp_write_bitmap_codecs_capability_set(s, settings))
			return FALSE;
	}

	if (!settings->ReceivedCapabilities[CAPSET_TYPE_FRAME_ACKNOWLEDGE])
		settings->FrameAcknowledge = 0;

	if (settings->FrameAcknowledge)
	{
		numberCapabilities++;

		if (!rdp_write_frame_acknowledge_capability_set(s, settings))
			return FALSE;
	}

	if (settings->ReceivedCapabilities[CAPSET_TYPE_BITMAP_CACHE_V3_CODEC_ID])
	{
		if (settings->BitmapCacheV3CodecId != 0)
		{
			numberCapabilities++;

			if (!rdp_write_bitmap_cache_v3_codec_id_capability_set(s, settings))
				return FALSE;
		}
	}

	em = Stream_GetPosition(s);
	Stream_SetPosition(s, lm); /* go back to lengthCombinedCapabilities */
	lengthCombinedCapabilities = (em - bm);
	if (lengthCombinedCapabilities > UINT16_MAX)
		return FALSE;
	Stream_Write_UINT16(
	    s, (UINT16)lengthCombinedCapabilities);         /* lengthCombinedCapabilities (2 bytes) */
	Stream_SetPosition(s, bm);                          /* go back to numberCapabilities */
	Stream_Write_UINT16(s, numberCapabilities);         /* numberCapabilities (2 bytes) */
#ifdef WITH_DEBUG_CAPABILITIES
	Stream_Seek_UINT16(s);
	rdp_print_capability_sets(s, numberCapabilities, FALSE);
	Stream_SetPosition(s, bm);
	Stream_Seek_UINT16(s);
#endif
	Stream_SetPosition(s, em);

	return TRUE;
}