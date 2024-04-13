exif_entry_dump (ExifEntry *e, unsigned int indent)
{
	char buf[1024];
	char value[1024];
	unsigned int l;

	if (!e)
		return;

	l = MIN(sizeof(buf)-1, 2*indent);
	memset(buf, ' ', l);
	buf[l] = '\0';

	printf ("%sTag: 0x%x ('%s')\n", buf, e->tag,
		exif_tag_get_name_in_ifd (e->tag, exif_entry_get_ifd(e)));
	printf ("%s  Format: %i ('%s')\n", buf, e->format,
		exif_format_get_name (e->format));
	printf ("%s  Components: %i\n", buf, (int) e->components);
	printf ("%s  Size: %i\n", buf, e->size);
	printf ("%s  Value: %s\n", buf, exif_entry_get_value (e, value, sizeof(value)));
}