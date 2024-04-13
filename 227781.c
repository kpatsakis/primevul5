static BOOL rdp_print_capability_sets(wStream* s, UINT16 numberCapabilities, BOOL receiving)
{
	UINT16 type;
	UINT16 length;
	BYTE *bm, *em;

	while (numberCapabilities > 0)
	{
		Stream_GetPointer(s, bm);
		rdp_read_capability_set_header(s, &length, &type);
		WLog_INFO(TAG, "%s ", receiving ? "Receiving" : "Sending");
		em = bm + length;

		if (Stream_GetRemainingLength(s) < (size_t)(length - 4))
		{
			WLog_ERR(TAG, "error processing stream");
			return FALSE;
		}

		switch (type)
		{
			case CAPSET_TYPE_GENERAL:
				if (!rdp_print_general_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_BITMAP:
				if (!rdp_print_bitmap_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_ORDER:
				if (!rdp_print_order_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_BITMAP_CACHE:
				if (!rdp_print_bitmap_cache_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_CONTROL:
				if (!rdp_print_control_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_ACTIVATION:
				if (!rdp_print_window_activation_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_POINTER:
				if (!rdp_print_pointer_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_SHARE:
				if (!rdp_print_share_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_COLOR_CACHE:
				if (!rdp_print_color_cache_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_SOUND:
				if (!rdp_print_sound_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_INPUT:
				if (!rdp_print_input_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_FONT:
				if (!rdp_print_font_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_BRUSH:
				if (!rdp_print_brush_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_GLYPH_CACHE:
				if (!rdp_print_glyph_cache_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_OFFSCREEN_CACHE:
				if (!rdp_print_offscreen_bitmap_cache_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_BITMAP_CACHE_HOST_SUPPORT:
				if (!rdp_print_bitmap_cache_host_support_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_BITMAP_CACHE_V2:
				if (!rdp_print_bitmap_cache_v2_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_VIRTUAL_CHANNEL:
				if (!rdp_print_virtual_channel_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_DRAW_NINE_GRID_CACHE:
				if (!rdp_print_draw_nine_grid_cache_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_DRAW_GDI_PLUS:
				if (!rdp_print_draw_gdiplus_cache_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_RAIL:
				if (!rdp_print_remote_programs_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_WINDOW:
				if (!rdp_print_window_list_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_COMP_DESK:
				if (!rdp_print_desktop_composition_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_MULTI_FRAGMENT_UPDATE:
				if (!rdp_print_multifragment_update_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_LARGE_POINTER:
				if (!rdp_print_large_pointer_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_SURFACE_COMMANDS:
				if (!rdp_print_surface_commands_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_BITMAP_CODECS:
				if (!rdp_print_bitmap_codecs_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_FRAME_ACKNOWLEDGE:
				if (!rdp_print_frame_acknowledge_capability_set(s, length))
					return FALSE;

				break;

			case CAPSET_TYPE_BITMAP_CACHE_V3_CODEC_ID:
				if (!rdp_print_bitmap_cache_v3_codec_id_capability_set(s, length))
					return FALSE;

				break;

			default:
				WLog_ERR(TAG, "unknown capability type %" PRIu16 "", type);
				break;
		}

		if (Stream_Pointer(s) != em)
		{
			WLog_ERR(TAG,
			         "incorrect offset, type:0x%04" PRIX16 " actual:%" PRIuz " expected:%" PRIuz "",
			         type, Stream_Pointer(s) - bm, em - bm);
		}

		Stream_SetPointer(s, em);
		numberCapabilities--;
	}

	return TRUE;
}