static avifBool avifCodecDecodeInputGetSamples(avifCodecDecodeInput * decodeInput, avifSampleTable * sampleTable, avifROData * rawInput)
{
    uint32_t sampleSizeIndex = 0;
    for (uint32_t chunkIndex = 0; chunkIndex < sampleTable->chunks.count; ++chunkIndex) {
        avifSampleTableChunk * chunk = &sampleTable->chunks.chunk[chunkIndex];

        // First, figure out how many samples are in this chunk
        uint32_t sampleCount = 0;
        for (int sampleToChunkIndex = sampleTable->sampleToChunks.count - 1; sampleToChunkIndex >= 0; --sampleToChunkIndex) {
            avifSampleTableSampleToChunk * sampleToChunk = &sampleTable->sampleToChunks.sampleToChunk[sampleToChunkIndex];
            if (sampleToChunk->firstChunk <= (chunkIndex + 1)) {
                sampleCount = sampleToChunk->samplesPerChunk;
                break;
            }
        }
        if (sampleCount == 0) {
            // chunks with 0 samples are invalid
            return AVIF_FALSE;
        }

        uint64_t sampleOffset = chunk->offset;
        for (uint32_t sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex) {
            uint32_t sampleSize = sampleTable->allSamplesSize;
            if (sampleSize == 0) {
                if (sampleSizeIndex >= sampleTable->sampleSizes.count) {
                    // We've run out of samples to sum
                    return AVIF_FALSE;
                }
                avifSampleTableSampleSize * sampleSizePtr = &sampleTable->sampleSizes.sampleSize[sampleSizeIndex];
                sampleSize = sampleSizePtr->size;
            }

            avifDecodeSample * sample = (avifDecodeSample *)avifArrayPushPtr(&decodeInput->samples);
            sample->data.data = rawInput->data + sampleOffset;
            sample->data.size = sampleSize;
            sample->sync = AVIF_FALSE; // to potentially be set to true following the outer loop

            if ((sampleOffset + sampleSize) > (uint64_t)rawInput->size) {
                return AVIF_FALSE;
            }

            sampleOffset += sampleSize;
            ++sampleSizeIndex;
        }
    }

    // Mark appropriate samples as sync
    for (uint32_t syncSampleIndex = 0; syncSampleIndex < sampleTable->syncSamples.count; ++syncSampleIndex) {
        uint32_t frameIndex = sampleTable->syncSamples.syncSample[syncSampleIndex].sampleNumber - 1; // sampleNumber is 1-based
        if (frameIndex < decodeInput->samples.count) {
            decodeInput->samples.sample[frameIndex].sync = AVIF_TRUE;
        }
    }

    // Assume frame 0 is sync, just in case the stss box is absent in the BMFF. (Unnecessary?)
    if (decodeInput->samples.count > 0) {
        decodeInput->samples.sample[0].sync = AVIF_TRUE;
    }
    return AVIF_TRUE;
}