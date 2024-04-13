static avifBool avifParseImageMirrorProperty(avifProperty * prop, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    avifImageMirror * imir = &prop->u.imir;
    CHECK(avifROStreamRead(&s, &imir->axis, 1)); // unsigned int (7) reserved = 0; unsigned int (1) axis;
    if ((imir->axis & 0xfe) != 0) {
        // reserved bits must be 0
        return AVIF_FALSE;
    }
    return AVIF_TRUE;
}