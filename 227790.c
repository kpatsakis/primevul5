static void rdp_read_bitmap_codec_guid(wStream* s, GUID* guid)
{
	BYTE g[16];
	Stream_Read(s, g, 16);
	guid->Data1 = (g[3] << 24) | (g[2] << 16) | (g[1] << 8) | g[0];
	guid->Data2 = (g[5] << 8) | g[4];
	guid->Data3 = (g[7] << 8) | g[6];
	guid->Data4[0] = g[8];
	guid->Data4[1] = g[9];
	guid->Data4[2] = g[10];
	guid->Data4[3] = g[11];
	guid->Data4[4] = g[12];
	guid->Data4[5] = g[13];
	guid->Data4[6] = g[14];
	guid->Data4[7] = g[15];
}