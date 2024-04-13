static avifBool avifParseImageRotationProperty(avifProperty * prop, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    avifImageRotation * irot = &prop->u.irot;
    CHECK(avifROStreamRead(&s, &irot->angle, 1)); // unsigned int (6) reserved = 0; unsigned int (2) angle;
    if ((irot->angle & 0xfc) != 0) {
        // reserved bits must be 0
        return AVIF_FALSE;
    }
    return AVIF_TRUE;
}