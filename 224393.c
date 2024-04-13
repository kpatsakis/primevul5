static avifBool avifParseItemInfoBox(avifMeta * meta, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    uint8_t version;
    CHECK(avifROStreamReadVersionAndFlags(&s, &version, NULL));
    uint32_t entryCount;
    if (version == 0) {
        uint16_t tmp;
        CHECK(avifROStreamReadU16(&s, &tmp)); // unsigned int(16) entry_count;
        entryCount = tmp;
    } else if (version == 1) {
        CHECK(avifROStreamReadU32(&s, &entryCount)); // unsigned int(32) entry_count;
    } else {
        return AVIF_FALSE;
    }

    for (uint32_t entryIndex = 0; entryIndex < entryCount; ++entryIndex) {
        avifBoxHeader infeHeader;
        CHECK(avifROStreamReadBoxHeader(&s, &infeHeader));

        if (!memcmp(infeHeader.type, "infe", 4)) {
            CHECK(avifParseItemInfoEntry(meta, avifROStreamCurrent(&s), infeHeader.size));
        } else {
            // These must all be type ipma
            return AVIF_FALSE;
        }

        CHECK(avifROStreamSkip(&s, infeHeader.size));
    }

    return AVIF_TRUE;
}