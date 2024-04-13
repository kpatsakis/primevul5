static avifBool avifParseSampleTableBox(avifTrack * track, const uint8_t * raw, size_t rawLen)
{
    if (track->sampleTable) {
        // A TrackBox may only have one SampleTable
        return AVIF_FALSE;
    }
    track->sampleTable = avifSampleTableCreate();

    BEGIN_STREAM(s, raw, rawLen);

    while (avifROStreamHasBytesLeft(&s, 1)) {
        avifBoxHeader header;
        CHECK(avifROStreamReadBoxHeader(&s, &header));

        if (!memcmp(header.type, "stco", 4)) {
            CHECK(avifParseChunkOffsetBox(track->sampleTable, AVIF_FALSE, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "co64", 4)) {
            CHECK(avifParseChunkOffsetBox(track->sampleTable, AVIF_TRUE, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "stsc", 4)) {
            CHECK(avifParseSampleToChunkBox(track->sampleTable, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "stsz", 4)) {
            CHECK(avifParseSampleSizeBox(track->sampleTable, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "stss", 4)) {
            CHECK(avifParseSyncSampleBox(track->sampleTable, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "stts", 4)) {
            CHECK(avifParseTimeToSampleBox(track->sampleTable, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "stsd", 4)) {
            CHECK(avifParseSampleDescriptionBox(track->sampleTable, avifROStreamCurrent(&s), header.size));
        }

        CHECK(avifROStreamSkip(&s, header.size));
    }
    return AVIF_TRUE;
}