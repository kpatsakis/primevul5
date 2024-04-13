static BOOL rdp_read_capability_sets(wStream* s, rdpSettings* settings, UINT16 numberCapabilities,
                                     UINT16 totalLength)
{
	BOOL treated;
	size_t start, end, len;
	UINT16 count = numberCapabilities;

	start = Stream_GetPosition(s);
	while (numberCapabilities > 0 && Stream_GetRemainingLength(s) >= 4)
	{
		UINT16 type;
		UINT16 length;
		BYTE* em;
		BYTE* bm = Stream_Pointer(s);
		rdp_read_capability_set_header(s, &length, &type);

		if (type < 32)
		{
			settings->ReceivedCapabilities[type] = TRUE;
		}
		else
		{
			WLog_WARN(TAG, "not handling capability type %" PRIu16 " yet", type);
		}

		em = bm + length;

		if (Stream_GetRemainingLength(s) + 4 < ((size_t)length))
		{
			WLog_ERR(TAG, "error processing stream");
			return FALSE;
		}

		treated = TRUE;

		switch (type)
		{
			case CAPSET_TYPE_GENERAL:
				if (!rdp_read_general_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_BITMAP:
				if (!rdp_read_bitmap_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_ORDER:
				if (!rdp_read_order_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_POINTER:
				if (!rdp_read_pointer_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_INPUT:
				if (!rdp_read_input_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_VIRTUAL_CHANNEL:
				if (!rdp_read_virtual_channel_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_SHARE:
				if (!rdp_read_share_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_COLOR_CACHE:
				if (!rdp_read_color_cache_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_FONT:
				if (!rdp_read_font_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_DRAW_GDI_PLUS:
				if (!rdp_read_draw_gdiplus_cache_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_RAIL:
				if (!rdp_read_remote_programs_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_WINDOW:
				if (!rdp_read_window_list_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_MULTI_FRAGMENT_UPDATE:
				if (!rdp_read_multifragment_update_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_LARGE_POINTER:
				if (!rdp_read_large_pointer_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_COMP_DESK:
				if (!rdp_read_desktop_composition_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_SURFACE_COMMANDS:
				if (!rdp_read_surface_commands_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_BITMAP_CODECS:
				if (!rdp_read_bitmap_codecs_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_FRAME_ACKNOWLEDGE:
				if (!rdp_read_frame_acknowledge_capability_set(s, length, settings))
					return FALSE;

				break;

			case CAPSET_TYPE_BITMAP_CACHE_V3_CODEC_ID:
				if (!rdp_read_bitmap_cache_v3_codec_id_capability_set(s, length, settings))
					return FALSE;

				break;

			default:
				treated = FALSE;
				break;
		}

		if (!treated)
		{
			if (settings->ServerMode)
			{
				/* treating capabilities that are supposed to be send only from the client */
				switch (type)
				{
					case CAPSET_TYPE_BITMAP_CACHE:
						if (!rdp_read_bitmap_cache_capability_set(s, length, settings))
							return FALSE;

						break;

					case CAPSET_TYPE_BITMAP_CACHE_V2:
						if (!rdp_read_bitmap_cache_v2_capability_set(s, length, settings))
							return FALSE;

						break;

					case CAPSET_TYPE_BRUSH:
						if (!rdp_read_brush_capability_set(s, length, settings))
							return FALSE;

						break;

					case CAPSET_TYPE_GLYPH_CACHE:
						if (!rdp_read_glyph_cache_capability_set(s, length, settings))
							return FALSE;

						break;

					case CAPSET_TYPE_OFFSCREEN_CACHE:
						if (!rdp_read_offscreen_bitmap_cache_capability_set(s, length, settings))
							return FALSE;

						break;

					case CAPSET_TYPE_SOUND:
						if (!rdp_read_sound_capability_set(s, length, settings))
							return FALSE;

						break;

					case CAPSET_TYPE_CONTROL:
						if (!rdp_read_control_capability_set(s, length, settings))
							return FALSE;

						break;

					case CAPSET_TYPE_ACTIVATION:
						if (!rdp_read_window_activation_capability_set(s, length, settings))
							return FALSE;

						break;

					case CAPSET_TYPE_DRAW_NINE_GRID_CACHE:
						if (!rdp_read_draw_nine_grid_cache_capability_set(s, length, settings))
							return FALSE;

						break;

					default:
						WLog_ERR(TAG, "capability %s(%" PRIu16 ") not expected from client",
						         get_capability_name(type), type);
						return FALSE;
				}
			}
			else
			{
				/* treating capabilities that are supposed to be send only from the server */
				switch (type)
				{
					case CAPSET_TYPE_BITMAP_CACHE_HOST_SUPPORT:
						if (!rdp_read_bitmap_cache_host_support_capability_set(s, length, settings))
							return FALSE;

						break;

					default:
						WLog_ERR(TAG, "capability %s(%" PRIu16 ") not expected from server",
						         get_capability_name(type), type);
						return FALSE;
				}
			}
		}

		if (Stream_Pointer(s) != em)
		{
			WLog_ERR(TAG,
			         "incorrect offset, type:0x%04" PRIX16 " actual:%" PRIuz " expected:%" PRIuz "",
			         type, Stream_Pointer(s) - bm, em - bm);
			Stream_SetPointer(s, em);
		}

		numberCapabilities--;
	}

	end = Stream_GetPosition(s);
	len = end - start;

	if (numberCapabilities)
	{
		WLog_ERR(TAG,
		         "strange we haven't read the number of announced capacity sets, read=%d "
		         "expected=%" PRIu16 "",
		         count - numberCapabilities, count);
	}

#ifdef WITH_DEBUG_CAPABILITIES
	{
		Stream_SetPosition(s, start);
		numberCapabilities = count;
		rdp_print_capability_sets(s, numberCapabilities, TRUE);
		Stream_SetPosition(s, end);
	}
#endif

	if (len > totalLength)
	{
		WLog_ERR(TAG, "Capability length expected %" PRIu16 ", actual %" PRIdz, totalLength, len);
		return FALSE;
	}
	return TRUE;
}