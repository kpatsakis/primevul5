static u32 ttxt_get_color(char *val)
{
	u32 r, g, b, a, res;
	r = g = b = a = 0;
	if (sscanf(val, "%x %x %x %x", &r, &g, &b, &a) != 4) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TXTIn] Warning: color badly formatted %s\n", val));
	}
	res = (a&0xFF);
	res<<=8;
	res |= (r&0xFF);
	res<<=8;
	res |= (g&0xFF);
	res<<=8;
	res |= (b&0xFF);
	return res;
}