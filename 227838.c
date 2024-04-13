static void rdp_write_bitmap_codec_guid(wStream* s, const GUID* guid)
{
	BYTE g[16];
	g[0] = guid->Data1 & 0xFF;
	g[1] = (guid->Data1 >> 8) & 0xFF;
	g[2] = (guid->Data1 >> 16) & 0xFF;
	g[3] = (guid->Data1 >> 24) & 0xFF;
	g[4] = (guid->Data2) & 0xFF;
	g[5] = (guid->Data2 >> 8) & 0xFF;
	g[6] = (guid->Data3) & 0xFF;
	g[7] = (guid->Data3 >> 8) & 0xFF;
	g[8] = guid->Data4[0];
	g[9] = guid->Data4[1];
	g[10] = guid->Data4[2];
	g[11] = guid->Data4[3];
	g[12] = guid->Data4[4];
	g[13] = guid->Data4[5];
	g[14] = guid->Data4[6];
	g[15] = guid->Data4[7];
	Stream_Write(s, g, 16);
}