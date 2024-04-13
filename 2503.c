static GF_Err gf_text_guess_format(const char *filename, u32 *fmt)
{
	char szLine[2048];
	u32 val;
	s32 uni_type;
	FILE *test = gf_fopen(filename, "rb");
	if (!test) return GF_URL_ERROR;
	uni_type = gf_text_get_utf_type(test);

	if (uni_type>1) {
		const u16 *sptr;
		char szUTF[1024];
		u32 read = (u32) gf_fread(szUTF, 1023, test);
		if ((s32) read < 0) {
			gf_fclose(test);
			return GF_IO_ERR;
		}
		szUTF[read]=0;
		sptr = (u16*)szUTF;
		/*read = (u32) */gf_utf8_wcstombs(szLine, read, &sptr);
	} else {
		val = (u32) gf_fread(szLine, 1024, test);
		if ((s32) val<0) return GF_IO_ERR;
		
		szLine[val]=0;
	}
	REM_TRAIL_MARKS(szLine, "\r\n\t ")

	*fmt = GF_TXTIN_MODE_NONE;
	if ((szLine[0]=='{') && strstr(szLine, "}{")) *fmt = GF_TXTIN_MODE_SUB;
	else if (szLine[0] == '<') {
		char *ext = gf_file_ext_start(filename);
		if (!strnicmp(ext, ".ttxt", 5)) *fmt = GF_TXTIN_MODE_TTXT;
		else if (!strnicmp(ext, ".ttml", 5)) *fmt = GF_TXTIN_MODE_TTML;
		ext = strstr(szLine, "?>");
		if (ext) ext += 2;
		if (ext && !ext[0]) {
			if (!gf_fgets(szLine, 2048, test))
				szLine[0] = '\0';
		}
		if (strstr(szLine, "x-quicktime-tx3g") || strstr(szLine, "text3GTrack")) *fmt = GF_TXTIN_MODE_TEXML;
		else if (strstr(szLine, "TextStream")) *fmt = GF_TXTIN_MODE_TTXT;
		else if (strstr(szLine, "tt")) *fmt = GF_TXTIN_MODE_TTML;
	}
	else if (strstr(szLine, "WEBVTT") )
		*fmt = GF_TXTIN_MODE_WEBVTT;
	else if (strstr(szLine, " --> ") )
		*fmt = GF_TXTIN_MODE_SRT; /* might want to change the default to WebVTT */

	else if (!strncmp(szLine, "FWS", 3) || !strncmp(szLine, "CWS", 3))
		*fmt = GF_TXTIN_MODE_SWF_SVG;

	gf_fclose(test);
	return GF_OK;
}