
GF_Err lsrc_box_size(GF_Box *s)
{
	GF_LASERConfigurationBox *ptr = (GF_LASERConfigurationBox *)s;
	ptr->size += ptr->hdr_size;
	return GF_OK;