
GF_Box *stsg_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SubTrackSampleGroupBox, GF_ISOM_BOX_TYPE_STSG);
	return (GF_Box *)tmp;