static avifBool avifParseImageSpatialExtentsProperty(avifProperty * prop, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);
    CHECK(avifROStreamReadAndEnforceVersion(&s, 0));

    avifImageSpatialExtents * ispe = &prop->u.ispe;
    CHECK(avifROStreamReadU32(&s, &ispe->width));
    CHECK(avifROStreamReadU32(&s, &ispe->height));
    return AVIF_TRUE;
}