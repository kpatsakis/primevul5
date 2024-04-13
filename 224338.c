static avifBool avifParsePixelInformationProperty(avifProperty * prop, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);
    CHECK(avifROStreamReadAndEnforceVersion(&s, 0));

    avifPixelInformationProperty * pixi = &prop->u.pixi;
    CHECK(avifROStreamRead(&s, &pixi->planeCount, 1)); // unsigned int (8) num_channels;
    if (pixi->planeCount > MAX_PIXI_PLANE_DEPTHS) {
        return AVIF_FALSE;
    }
    for (uint8_t i = 0; i < pixi->planeCount; ++i) {
        CHECK(avifROStreamRead(&s, &pixi->planeDepths[i], 1)); // unsigned int (8) bits_per_channel;
    }
    return AVIF_TRUE;
}