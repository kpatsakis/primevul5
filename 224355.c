static avifBool avifParseTrackBox(avifDecoderData * data, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    avifTrack * track = avifDecoderDataCreateTrack(data);

    while (avifROStreamHasBytesLeft(&s, 1)) {
        avifBoxHeader header;
        CHECK(avifROStreamReadBoxHeader(&s, &header));

        if (!memcmp(header.type, "tkhd", 4)) {
            CHECK(avifParseTrackHeaderBox(track, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "meta", 4)) {
            CHECK(avifParseMetaBox(track->meta, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "mdia", 4)) {
            CHECK(avifParseMediaBox(track, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "tref", 4)) {
            CHECK(avifTrackReferenceBox(track, avifROStreamCurrent(&s), header.size));
        }

        CHECK(avifROStreamSkip(&s, header.size));
    }
    return AVIF_TRUE;
}