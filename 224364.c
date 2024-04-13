static avifBool avifParseSampleDescriptionBox(avifSampleTable * sampleTable, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    CHECK(avifROStreamReadAndEnforceVersion(&s, 0));

    uint32_t entryCount;
    CHECK(avifROStreamReadU32(&s, &entryCount)); // unsigned int(32) entry_count;

    for (uint32_t i = 0; i < entryCount; ++i) {
        avifBoxHeader sampleEntryHeader;
        CHECK(avifROStreamReadBoxHeader(&s, &sampleEntryHeader));

        avifSampleDescription * description = (avifSampleDescription *)avifArrayPushPtr(&sampleTable->sampleDescriptions);
        avifArrayCreate(&description->properties, sizeof(avifProperty), 16);
        memcpy(description->format, sampleEntryHeader.type, sizeof(description->format));
        size_t remainingBytes = avifROStreamRemainingBytes(&s);
        if (!memcmp(description->format, "av01", 4) && (remainingBytes > VISUALSAMPLEENTRY_SIZE)) {
            CHECK(avifParseItemPropertyContainerBox(
                &description->properties, avifROStreamCurrent(&s) + VISUALSAMPLEENTRY_SIZE, remainingBytes - VISUALSAMPLEENTRY_SIZE));
        }

        CHECK(avifROStreamSkip(&s, sampleEntryHeader.size));
    }
    return AVIF_TRUE;
}