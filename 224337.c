static avifBool avifParseAV1CodecConfigurationBox(const uint8_t * raw, size_t rawLen, avifCodecConfigurationBox * av1C)
{
    BEGIN_STREAM(s, raw, rawLen);

    uint8_t markerAndVersion = 0;
    CHECK(avifROStreamRead(&s, &markerAndVersion, 1));
    uint8_t seqProfileAndIndex = 0;
    CHECK(avifROStreamRead(&s, &seqProfileAndIndex, 1));
    uint8_t rawFlags = 0;
    CHECK(avifROStreamRead(&s, &rawFlags, 1));

    if (markerAndVersion != 0x81) {
        // Marker and version must both == 1
        return AVIF_FALSE;
    }

    av1C->seqProfile = (seqProfileAndIndex >> 5) & 0x7;    // unsigned int (3) seq_profile;
    av1C->seqLevelIdx0 = (seqProfileAndIndex >> 0) & 0x1f; // unsigned int (5) seq_level_idx_0;
    av1C->seqTier0 = (rawFlags >> 7) & 0x1;                // unsigned int (1) seq_tier_0;
    av1C->highBitdepth = (rawFlags >> 6) & 0x1;            // unsigned int (1) high_bitdepth;
    av1C->twelveBit = (rawFlags >> 5) & 0x1;               // unsigned int (1) twelve_bit;
    av1C->monochrome = (rawFlags >> 4) & 0x1;              // unsigned int (1) monochrome;
    av1C->chromaSubsamplingX = (rawFlags >> 3) & 0x1;      // unsigned int (1) chroma_subsampling_x;
    av1C->chromaSubsamplingY = (rawFlags >> 2) & 0x1;      // unsigned int (1) chroma_subsampling_y;
    av1C->chromaSamplePosition = (rawFlags >> 0) & 0x3;    // unsigned int (2) chroma_sample_position;
    return AVIF_TRUE;
}