static avifBool avifParseItemPropertiesBox(avifMeta * meta, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    avifBoxHeader ipcoHeader;
    CHECK(avifROStreamReadBoxHeader(&s, &ipcoHeader));
    if (memcmp(ipcoHeader.type, "ipco", 4) != 0) {
        return AVIF_FALSE;
    }

    // Read all item properties inside of ItemPropertyContainerBox
    CHECK(avifParseItemPropertyContainerBox(&meta->properties, avifROStreamCurrent(&s), ipcoHeader.size));
    CHECK(avifROStreamSkip(&s, ipcoHeader.size));

    // Now read all ItemPropertyAssociation until the end of the box, and make associations
    while (avifROStreamHasBytesLeft(&s, 1)) {
        avifBoxHeader ipmaHeader;
        CHECK(avifROStreamReadBoxHeader(&s, &ipmaHeader));

        if (!memcmp(ipmaHeader.type, "ipma", 4)) {
            CHECK(avifParseItemPropertyAssociation(meta, avifROStreamCurrent(&s), ipmaHeader.size));
        } else {
            // These must all be type ipma
            return AVIF_FALSE;
        }

        CHECK(avifROStreamSkip(&s, ipmaHeader.size));
    }
    return AVIF_TRUE;
}