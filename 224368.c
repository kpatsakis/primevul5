static avifBool avifParseMediaBox(avifTrack * track, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    while (avifROStreamHasBytesLeft(&s, 1)) {
        avifBoxHeader header;
        CHECK(avifROStreamReadBoxHeader(&s, &header));

        if (!memcmp(header.type, "mdhd", 4)) {
            CHECK(avifParseMediaHeaderBox(track, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "minf", 4)) {
            CHECK(avifParseMediaInformationBox(track, avifROStreamCurrent(&s), header.size));
        }

        CHECK(avifROStreamSkip(&s, header.size));
    }
    return AVIF_TRUE;
}