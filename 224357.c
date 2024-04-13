static avifBool avifParseMetaBox(avifMeta * meta, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    CHECK(avifROStreamReadAndEnforceVersion(&s, 0));

    ++meta->idatID; // for tracking idat

    while (avifROStreamHasBytesLeft(&s, 1)) {
        avifBoxHeader header;
        CHECK(avifROStreamReadBoxHeader(&s, &header));

        if (!memcmp(header.type, "iloc", 4)) {
            CHECK(avifParseItemLocationBox(meta, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "pitm", 4)) {
            CHECK(avifParsePrimaryItemBox(meta, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "idat", 4)) {
            CHECK(avifParseItemDataBox(meta, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "iprp", 4)) {
            CHECK(avifParseItemPropertiesBox(meta, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "iinf", 4)) {
            CHECK(avifParseItemInfoBox(meta, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "iref", 4)) {
            CHECK(avifParseItemReferenceBox(meta, avifROStreamCurrent(&s), header.size));
        }

        CHECK(avifROStreamSkip(&s, header.size));
    }
    return AVIF_TRUE;
}