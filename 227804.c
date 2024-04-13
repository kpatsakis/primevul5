static BOOL rdp_print_order_capability_set(wStream* s, UINT16 length)
{
	BYTE terminalDescriptor[16];
	UINT32 pad4OctetsA;
	UINT16 desktopSaveXGranularity;
	UINT16 desktopSaveYGranularity;
	UINT16 pad2OctetsA;
	UINT16 maximumOrderLevel;
	UINT16 numberFonts;
	UINT16 orderFlags;
	BYTE orderSupport[32];
	UINT16 textFlags;
	UINT16 orderSupportExFlags;
	UINT32 pad4OctetsB;
	UINT32 desktopSaveSize;
	UINT16 pad2OctetsC;
	UINT16 pad2OctetsD;
	UINT16 textANSICodePage;
	UINT16 pad2OctetsE;
	WLog_INFO(TAG, "OrderCapabilitySet (length %" PRIu16 "):", length);

	if (length < 88)
		return FALSE;

	Stream_Read(s, terminalDescriptor, 16);         /* terminalDescriptor (16 bytes) */
	Stream_Read_UINT32(s, pad4OctetsA);             /* pad4OctetsA (4 bytes) */
	Stream_Read_UINT16(s, desktopSaveXGranularity); /* desktopSaveXGranularity (2 bytes) */
	Stream_Read_UINT16(s, desktopSaveYGranularity); /* desktopSaveYGranularity (2 bytes) */
	Stream_Read_UINT16(s, pad2OctetsA);             /* pad2OctetsA (2 bytes) */
	Stream_Read_UINT16(s, maximumOrderLevel);       /* maximumOrderLevel (2 bytes) */
	Stream_Read_UINT16(s, numberFonts);             /* numberFonts (2 bytes) */
	Stream_Read_UINT16(s, orderFlags);              /* orderFlags (2 bytes) */
	Stream_Read(s, orderSupport, 32);               /* orderSupport (32 bytes) */
	Stream_Read_UINT16(s, textFlags);               /* textFlags (2 bytes) */
	Stream_Read_UINT16(s, orderSupportExFlags);     /* orderSupportExFlags (2 bytes) */
	Stream_Read_UINT32(s, pad4OctetsB);             /* pad4OctetsB (4 bytes) */
	Stream_Read_UINT32(s, desktopSaveSize);         /* desktopSaveSize (4 bytes) */
	Stream_Read_UINT16(s, pad2OctetsC);             /* pad2OctetsC (2 bytes) */
	Stream_Read_UINT16(s, pad2OctetsD);             /* pad2OctetsD (2 bytes) */
	Stream_Read_UINT16(s, textANSICodePage);        /* textANSICodePage (2 bytes) */
	Stream_Read_UINT16(s, pad2OctetsE);             /* pad2OctetsE (2 bytes) */
	WLog_INFO(TAG, "\tpad4OctetsA: 0x%08" PRIX32 "", pad4OctetsA);
	WLog_INFO(TAG, "\tdesktopSaveXGranularity: 0x%04" PRIX16 "", desktopSaveXGranularity);
	WLog_INFO(TAG, "\tdesktopSaveYGranularity: 0x%04" PRIX16 "", desktopSaveYGranularity);
	WLog_INFO(TAG, "\tpad2OctetsA: 0x%04" PRIX16 "", pad2OctetsA);
	WLog_INFO(TAG, "\tmaximumOrderLevel: 0x%04" PRIX16 "", maximumOrderLevel);
	WLog_INFO(TAG, "\tnumberFonts: 0x%04" PRIX16 "", numberFonts);
	WLog_INFO(TAG, "\torderFlags: 0x%04" PRIX16 "", orderFlags);
	WLog_INFO(TAG, "\torderSupport:");
	WLog_INFO(TAG, "\t\tDSTBLT: %" PRIu8 "", orderSupport[NEG_DSTBLT_INDEX]);
	WLog_INFO(TAG, "\t\tPATBLT: %" PRIu8 "", orderSupport[NEG_PATBLT_INDEX]);
	WLog_INFO(TAG, "\t\tSCRBLT: %" PRIu8 "", orderSupport[NEG_SCRBLT_INDEX]);
	WLog_INFO(TAG, "\t\tMEMBLT: %" PRIu8 "", orderSupport[NEG_MEMBLT_INDEX]);
	WLog_INFO(TAG, "\t\tMEM3BLT: %" PRIu8 "", orderSupport[NEG_MEM3BLT_INDEX]);
	WLog_INFO(TAG, "\t\tATEXTOUT: %" PRIu8 "", orderSupport[NEG_ATEXTOUT_INDEX]);
	WLog_INFO(TAG, "\t\tAEXTTEXTOUT: %" PRIu8 "", orderSupport[NEG_AEXTTEXTOUT_INDEX]);
	WLog_INFO(TAG, "\t\tDRAWNINEGRID: %" PRIu8 "", orderSupport[NEG_DRAWNINEGRID_INDEX]);
	WLog_INFO(TAG, "\t\tLINETO: %" PRIu8 "", orderSupport[NEG_LINETO_INDEX]);
	WLog_INFO(TAG, "\t\tMULTI_DRAWNINEGRID: %" PRIu8 "",
	          orderSupport[NEG_MULTI_DRAWNINEGRID_INDEX]);
	WLog_INFO(TAG, "\t\tOPAQUE_RECT: %" PRIu8 "", orderSupport[NEG_OPAQUE_RECT_INDEX]);
	WLog_INFO(TAG, "\t\tSAVEBITMAP: %" PRIu8 "", orderSupport[NEG_SAVEBITMAP_INDEX]);
	WLog_INFO(TAG, "\t\tWTEXTOUT: %" PRIu8 "", orderSupport[NEG_WTEXTOUT_INDEX]);
	WLog_INFO(TAG, "\t\tMEMBLT_V2: %" PRIu8 "", orderSupport[NEG_MEMBLT_V2_INDEX]);
	WLog_INFO(TAG, "\t\tMEM3BLT_V2: %" PRIu8 "", orderSupport[NEG_MEM3BLT_V2_INDEX]);
	WLog_INFO(TAG, "\t\tMULTIDSTBLT: %" PRIu8 "", orderSupport[NEG_MULTIDSTBLT_INDEX]);
	WLog_INFO(TAG, "\t\tMULTIPATBLT: %" PRIu8 "", orderSupport[NEG_MULTIPATBLT_INDEX]);
	WLog_INFO(TAG, "\t\tMULTISCRBLT: %" PRIu8 "", orderSupport[NEG_MULTISCRBLT_INDEX]);
	WLog_INFO(TAG, "\t\tMULTIOPAQUERECT: %" PRIu8 "", orderSupport[NEG_MULTIOPAQUERECT_INDEX]);
	WLog_INFO(TAG, "\t\tFAST_INDEX: %" PRIu8 "", orderSupport[NEG_FAST_INDEX_INDEX]);
	WLog_INFO(TAG, "\t\tPOLYGON_SC: %" PRIu8 "", orderSupport[NEG_POLYGON_SC_INDEX]);
	WLog_INFO(TAG, "\t\tPOLYGON_CB: %" PRIu8 "", orderSupport[NEG_POLYGON_CB_INDEX]);
	WLog_INFO(TAG, "\t\tPOLYLINE: %" PRIu8 "", orderSupport[NEG_POLYLINE_INDEX]);
	WLog_INFO(TAG, "\t\tUNUSED23: %" PRIu8 "", orderSupport[NEG_UNUSED23_INDEX]);
	WLog_INFO(TAG, "\t\tFAST_GLYPH: %" PRIu8 "", orderSupport[NEG_FAST_GLYPH_INDEX]);
	WLog_INFO(TAG, "\t\tELLIPSE_SC: %" PRIu8 "", orderSupport[NEG_ELLIPSE_SC_INDEX]);
	WLog_INFO(TAG, "\t\tELLIPSE_CB: %" PRIu8 "", orderSupport[NEG_ELLIPSE_CB_INDEX]);
	WLog_INFO(TAG, "\t\tGLYPH_INDEX: %" PRIu8 "", orderSupport[NEG_GLYPH_INDEX_INDEX]);
	WLog_INFO(TAG, "\t\tGLYPH_WEXTTEXTOUT: %" PRIu8 "", orderSupport[NEG_GLYPH_WEXTTEXTOUT_INDEX]);
	WLog_INFO(TAG, "\t\tGLYPH_WLONGTEXTOUT: %" PRIu8 "",
	          orderSupport[NEG_GLYPH_WLONGTEXTOUT_INDEX]);
	WLog_INFO(TAG, "\t\tGLYPH_WLONGEXTTEXTOUT: %" PRIu8 "",
	          orderSupport[NEG_GLYPH_WLONGEXTTEXTOUT_INDEX]);
	WLog_INFO(TAG, "\t\tUNUSED31: %" PRIu8 "", orderSupport[NEG_UNUSED31_INDEX]);
	WLog_INFO(TAG, "\ttextFlags: 0x%04" PRIX16 "", textFlags);
	WLog_INFO(TAG, "\torderSupportExFlags: 0x%04" PRIX16 "", orderSupportExFlags);
	WLog_INFO(TAG, "\tpad4OctetsB: 0x%08" PRIX32 "", pad4OctetsB);
	WLog_INFO(TAG, "\tdesktopSaveSize: 0x%08" PRIX32 "", desktopSaveSize);
	WLog_INFO(TAG, "\tpad2OctetsC: 0x%04" PRIX16 "", pad2OctetsC);
	WLog_INFO(TAG, "\tpad2OctetsD: 0x%04" PRIX16 "", pad2OctetsD);
	WLog_INFO(TAG, "\ttextANSICodePage: 0x%04" PRIX16 "", textANSICodePage);
	WLog_INFO(TAG, "\tpad2OctetsE: 0x%04" PRIX16 "", pad2OctetsE);
	return TRUE;
}