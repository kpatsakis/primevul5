static avifBool avifParseTrackHeaderBox(avifTrack * track, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    uint8_t version;
    CHECK(avifROStreamReadVersionAndFlags(&s, &version, NULL));

    uint32_t ignored32, trackID;
    uint64_t ignored64;
    if (version == 1) {
        CHECK(avifROStreamReadU64(&s, &ignored64)); // unsigned int(64) creation_time;
        CHECK(avifROStreamReadU64(&s, &ignored64)); // unsigned int(64) modification_time;
        CHECK(avifROStreamReadU32(&s, &trackID));   // unsigned int(32) track_ID;
        CHECK(avifROStreamReadU32(&s, &ignored32)); // const unsigned int(32) reserved = 0;
        CHECK(avifROStreamReadU64(&s, &ignored64)); // unsigned int(64) duration;
    } else if (version == 0) {
        CHECK(avifROStreamReadU32(&s, &ignored32)); // unsigned int(32) creation_time;
        CHECK(avifROStreamReadU32(&s, &ignored32)); // unsigned int(32) modification_time;
        CHECK(avifROStreamReadU32(&s, &trackID));   // unsigned int(32) track_ID;
        CHECK(avifROStreamReadU32(&s, &ignored32)); // const unsigned int(32) reserved = 0;
        CHECK(avifROStreamReadU32(&s, &ignored32)); // unsigned int(32) duration;
    } else {
        // Unsupported version
        return AVIF_FALSE;
    }

    // Skipping the following 52 bytes here:
    // ------------------------------------
    // const unsigned int(32)[2] reserved = 0;
    // template int(16) layer = 0;
    // template int(16) alternate_group = 0;
    // template int(16) volume = {if track_is_audio 0x0100 else 0};
    // const unsigned int(16) reserved = 0;
    // template int(32)[9] matrix= { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 }; // unity matrix
    CHECK(avifROStreamSkip(&s, 52));

    uint32_t width, height;
    CHECK(avifROStreamReadU32(&s, &width));  // unsigned int(32) width;
    CHECK(avifROStreamReadU32(&s, &height)); // unsigned int(32) height;
    track->width = width >> 16;
    track->height = height >> 16;

    // TODO: support scaling based on width/height track header info?

    track->id = trackID;
    return AVIF_TRUE;
}