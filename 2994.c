void tfhd_box_del(GF_Box *s)
{
	GF_TrackFragmentHeaderBox *ptr = (GF_TrackFragmentHeaderBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}