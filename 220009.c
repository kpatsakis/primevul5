grabData(rpm_tagtype_t type, rpm_constdata_t p, rpm_count_t c, int * lengthPtr)
{
    rpm_data_t data = NULL;
    int length;

    length = dataLength(type, p, c, 0, NULL);
    if (length > 0) {
	data = xmalloc(length);
	copyData(type, data, p, c, length);
    }

    if (lengthPtr)
	*lengthPtr = length;
    return data;
}