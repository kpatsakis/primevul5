static avifBool avifParseSyncSampleBox(avifSampleTable * sampleTable, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    CHECK(avifROStreamReadAndEnforceVersion(&s, 0));

    uint32_t entryCount;
    CHECK(avifROStreamReadU32(&s, &entryCount)); // unsigned int(32) entry_count;

    for (uint32_t i = 0; i < entryCount; ++i) {
        uint32_t sampleNumber = 0;
        CHECK(avifROStreamReadU32(&s, &sampleNumber)); // unsigned int(32) sample_number;
        avifSyncSample * syncSample = (avifSyncSample *)avifArrayPushPtr(&sampleTable->syncSamples);
        syncSample->sampleNumber = sampleNumber;
    }
    return AVIF_TRUE;
}