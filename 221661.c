POINTER_COLOR_UPDATE* update_read_pointer_color(rdpUpdate* update, wStream* s, BYTE xorBpp)
{
	POINTER_COLOR_UPDATE* pointer_color = calloc(1, sizeof(POINTER_COLOR_UPDATE));

	if (!pointer_color)
		goto fail;

	if (!_update_read_pointer_color(s, pointer_color, xorBpp))
		goto fail;

	return pointer_color;
fail:
	free_pointer_color_update(update->context, pointer_color);
	return NULL;
}