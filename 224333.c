static avifBool avifTrackReferenceBox(avifTrack * track, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    while (avifROStreamHasBytesLeft(&s, 1)) {
        avifBoxHeader header;
        CHECK(avifROStreamReadBoxHeader(&s, &header));

        if (!memcmp(header.type, "auxl", 4)) {
            uint32_t toID;
            CHECK(avifROStreamReadU32(&s, &toID));                       // unsigned int(32) track_IDs[]
            CHECK(avifROStreamSkip(&s, header.size - sizeof(uint32_t))); // just take the first one
            track->auxForID = toID;
        } else {
            CHECK(avifROStreamSkip(&s, header.size));
        }
    }
    return AVIF_TRUE;
}