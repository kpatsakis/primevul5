POINTER_NEW_UPDATE* update_read_pointer_new(rdpUpdate* update, wStream* s)
{
	POINTER_NEW_UPDATE* pointer_new = calloc(1, sizeof(POINTER_NEW_UPDATE));

	if (!pointer_new)
		goto fail;

	if (Stream_GetRemainingLength(s) < 2)
		goto fail;

	Stream_Read_UINT16(s, pointer_new->xorBpp); /* xorBpp (2 bytes) */

	if ((pointer_new->xorBpp < 1) || (pointer_new->xorBpp > 32))
	{
		WLog_ERR(TAG, "invalid xorBpp %" PRIu32 "", pointer_new->xorBpp);
		goto fail;
	}

	if (!_update_read_pointer_color(s, &pointer_new->colorPtrAttr,
	                                pointer_new->xorBpp)) /* colorPtrAttr */
		goto fail;

	return pointer_new;
fail:
	free_pointer_new_update(update->context, pointer_new);
	return NULL;
}