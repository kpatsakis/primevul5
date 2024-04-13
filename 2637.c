GF_Err meta_box_read(GF_Box *s, GF_BitStream *bs)
{
	u64 pos = gf_bs_get_position(bs);
	u64 size = s->size;
	GF_Err e = gf_isom_box_array_read(s, bs);
	/*try to hack around QT files which don't use a full box for meta, rewind 4 bytes*/
	if (e && (pos>4) ) {
		gf_bs_seek(bs, pos-4);
		meta_reset(s);
		s->size = size+4;
		e = gf_isom_box_array_read(s, bs);
	}
	return e;
}