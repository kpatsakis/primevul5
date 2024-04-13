static const char* general_name_type_label(int general_name_type)
{
	if ((0 <= general_name_type) &&
	    ((size_t)general_name_type < ARRAYSIZE(general_name_type_labels)))
	{
		return general_name_type_labels[general_name_type];
	}
	else
	{
		static char buffer[80];
		sprintf(buffer, "Unknown general name type (%d)", general_name_type);
		return buffer;
	}
}