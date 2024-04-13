static avifBool avifParseItemPropertyContainerBox(avifPropertyArray * properties, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    while (avifROStreamHasBytesLeft(&s, 1)) {
        avifBoxHeader header;
        CHECK(avifROStreamReadBoxHeader(&s, &header));

        int propertyIndex = avifArrayPushIndex(properties);
        avifProperty * prop = &properties->prop[propertyIndex];
        memcpy(prop->type, header.type, 4);
        if (!memcmp(header.type, "ispe", 4)) {
            CHECK(avifParseImageSpatialExtentsProperty(prop, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "auxC", 4)) {
            CHECK(avifParseAuxiliaryTypeProperty(prop, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "colr", 4)) {
            CHECK(avifParseColourInformationBox(prop, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "av1C", 4)) {
            CHECK(avifParseAV1CodecConfigurationBoxProperty(prop, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "pasp", 4)) {
            CHECK(avifParsePixelAspectRatioBoxProperty(prop, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "clap", 4)) {
            CHECK(avifParseCleanApertureBoxProperty(prop, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "irot", 4)) {
            CHECK(avifParseImageRotationProperty(prop, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "imir", 4)) {
            CHECK(avifParseImageMirrorProperty(prop, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "pixi", 4)) {
            CHECK(avifParsePixelInformationProperty(prop, avifROStreamCurrent(&s), header.size));
        }

        CHECK(avifROStreamSkip(&s, header.size));
    }
    return AVIF_TRUE;
}