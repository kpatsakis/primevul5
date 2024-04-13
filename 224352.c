static avifBool avifParseSampleSizeBox(avifSampleTable * sampleTable, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    CHECK(avifROStreamReadAndEnforceVersion(&s, 0));

    uint32_t allSamplesSize, sampleCount;
    CHECK(avifROStreamReadU32(&s, &allSamplesSize)); // unsigned int(32) sample_size;
    CHECK(avifROStreamReadU32(&s, &sampleCount));    // unsigned int(32) sample_count;

    if (allSamplesSize > 0) {
        sampleTable->allSamplesSize = allSamplesSize;
    } else {
        for (uint32_t i = 0; i < sampleCount; ++i) {
            avifSampleTableSampleSize * sampleSize = (avifSampleTableSampleSize *)avifArrayPushPtr(&sampleTable->sampleSizes);
            CHECK(avifROStreamReadU32(&s, &sampleSize->size)); // unsigned int(32) entry_size;
        }
    }
    return AVIF_TRUE;
}