static BOOL rdp_write_demand_active(wStream* s, rdpSettings* settings)
{
	size_t bm, em, lm;
	UINT16 numberCapabilities;
	size_t lengthCombinedCapabilities;

	if (!Stream_EnsureRemainingCapacity(s, 64))
		return FALSE;

	Stream_Write_UINT32(s, settings->ShareId); /* shareId (4 bytes) */
	Stream_Write_UINT16(s, 4);                 /* lengthSourceDescriptor (2 bytes) */
	lm = Stream_GetPosition(s);
	Stream_Seek_UINT16(s);     /* lengthCombinedCapabilities (2 bytes) */
	Stream_Write(s, "RDP", 4); /* sourceDescriptor */
	bm = Stream_GetPosition(s);
	Stream_Seek_UINT16(s);     /* numberCapabilities (2 bytes) */
	Stream_Write_UINT16(s, 0); /* pad2Octets (2 bytes) */
	numberCapabilities = 14;

	if (!rdp_write_general_capability_set(s, settings) ||
	    !rdp_write_bitmap_capability_set(s, settings) ||
	    !rdp_write_order_capability_set(s, settings) ||
	    !rdp_write_pointer_capability_set(s, settings) ||
	    !rdp_write_input_capability_set(s, settings) ||
	    !rdp_write_virtual_channel_capability_set(s, settings) ||
	    !rdp_write_share_capability_set(s, settings) ||
	    !rdp_write_font_capability_set(s, settings) ||
	    !rdp_write_multifragment_update_capability_set(s, settings) ||
	    !rdp_write_large_pointer_capability_set(s, settings) ||
	    !rdp_write_desktop_composition_capability_set(s, settings) ||
	    !rdp_write_surface_commands_capability_set(s, settings) ||
	    !rdp_write_bitmap_codecs_capability_set(s, settings) ||
	    !rdp_write_frame_acknowledge_capability_set(s, settings))
	{
		return FALSE;
	}

	if (settings->BitmapCachePersistEnabled)
	{
		numberCapabilities++;

		if (!rdp_write_bitmap_cache_host_support_capability_set(s, settings))
			return FALSE;
	}

	if (settings->RemoteApplicationMode)
	{
		numberCapabilities += 2;

		if (!rdp_write_remote_programs_capability_set(s, settings) ||
		    !rdp_write_window_list_capability_set(s, settings))
			return FALSE;
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
	Stream_Write_UINT32(s, 0); /* sessionId */
	return TRUE;
}