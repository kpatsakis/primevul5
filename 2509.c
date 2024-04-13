static u32 tx3g_get_color(char *value)
{
	u32 r, g, b, a;
	u32 res, v;
	r = g = b = a = 0;
	if (sscanf(value, "%u%%, %u%%, %u%%, %u%%", &r, &g, &b, &a) != 4) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("Warning: color badly formatted\n"));
	}
	v = (u32) (a*255/100);
	res = (v&0xFF);
	res<<=8;
	v = (u32) (r*255/100);
	res |= (v&0xFF);
	res<<=8;
	v = (u32) (g*255/100);
	res |= (v&0xFF);
	res<<=8;
	v = (u32) (b*255/100);
	res |= (v&0xFF);
	return res;
}