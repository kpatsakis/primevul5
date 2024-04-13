static avifBool avifParse(avifDecoderData * data, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    while (avifROStreamHasBytesLeft(&s, 1)) {
        avifBoxHeader header;
        CHECK(avifROStreamReadBoxHeader(&s, &header));

        if (!memcmp(header.type, "ftyp", 4)) {
            CHECK(avifParseFileTypeBox(&data->ftyp, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "meta", 4)) {
            CHECK(avifParseMetaBox(data->meta, avifROStreamCurrent(&s), header.size));
        } else if (!memcmp(header.type, "moov", 4)) {
            CHECK(avifParseMoovBox(data, avifROStreamCurrent(&s), header.size));
        }

        CHECK(avifROStreamSkip(&s, header.size));
    }
    return AVIF_TRUE;
}