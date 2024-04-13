s32 gf_text_get_utf_type(FILE *in_src)
{
	u32 read;
	unsigned char BOM[5];
	read = (u32) gf_fread(BOM, 5, in_src);
	if ((s32) read < 1)
		return -1;

	if ((BOM[0]==0xFF) && (BOM[1]==0xFE)) {
		/*UTF32 not supported*/
		if (!BOM[2] && !BOM[3]) return -1;
		gf_fseek(in_src, 2, SEEK_SET);
		return 3;
	}
	if ((BOM[0]==0xFE) && (BOM[1]==0xFF)) {
		/*UTF32 not supported*/
		if (!BOM[2] && !BOM[3]) return -1;
		gf_fseek(in_src, 2, SEEK_SET);
		return 2;
	} else if ((BOM[0]==0xEF) && (BOM[1]==0xBB) && (BOM[2]==0xBF)) {
		gf_fseek(in_src, 3, SEEK_SET);
		return 1;
	}
	if (BOM[0]<0x80) {
		gf_fseek(in_src, 0, SEEK_SET);
		return 0;
	}
	return -1;
}