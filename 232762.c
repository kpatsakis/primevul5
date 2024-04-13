static char* gdi_rect_str(char* buffer, size_t size, const HGDI_RECT rect)
{
	_snprintf(buffer, size - 1,
	          "[top/left=%" PRId32 "x%" PRId32 "-bottom/right%" PRId32 "x%" PRId32 "]", rect->top,
	          rect->left, rect->bottom, rect->right);
	if (size > 1)
		buffer[size - 1] = '\0'

		    return buffer;
}