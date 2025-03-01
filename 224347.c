avifResult avifDecoderReset(avifDecoder * decoder)
{
    avifDecoderData * data = decoder->data;
    if (!data) {
        // Nothing to reset.
        return AVIF_RESULT_OK;
    }

    memset(&data->colorGrid, 0, sizeof(data->colorGrid));
    memset(&data->alphaGrid, 0, sizeof(data->alphaGrid));
    avifDecoderDataClearTiles(data);

    // Prepare / cleanup decoded image state
    if (decoder->image) {
        avifImageDestroy(decoder->image);
    }
    decoder->image = avifImageCreateEmpty();
    data->cicpSet = AVIF_FALSE;

    memset(&decoder->ioStats, 0, sizeof(decoder->ioStats));

    // -----------------------------------------------------------------------
    // Build decode input

    data->sourceSampleTable = NULL; // Reset
    if (decoder->requestedSource == AVIF_DECODER_SOURCE_AUTO) {
        if (data->tracks.count > 0) {
            data->source = AVIF_DECODER_SOURCE_TRACKS;
        } else {
            data->source = AVIF_DECODER_SOURCE_PRIMARY_ITEM;
        }
    } else {
        data->source = decoder->requestedSource;
    }

    const avifPropertyArray * colorProperties = NULL;
    if (data->source == AVIF_DECODER_SOURCE_TRACKS) {
        avifTrack * colorTrack = NULL;
        avifTrack * alphaTrack = NULL;

        // Find primary track - this probably needs some better detection
        uint32_t colorTrackIndex = 0;
        for (; colorTrackIndex < decoder->data->tracks.count; ++colorTrackIndex) {
            avifTrack * track = &decoder->data->tracks.track[colorTrackIndex];
            if (!track->sampleTable) {
                continue;
            }
            if (!track->id) { // trak box might be missing a tkhd box inside, skip it
                continue;
            }
            if (!track->sampleTable->chunks.count) {
                continue;
            }
            if (!avifSampleTableHasFormat(track->sampleTable, "av01")) {
                continue;
            }
            if (track->auxForID != 0) {
                continue;
            }

            // Found one!
            break;
        }
        if (colorTrackIndex == decoder->data->tracks.count) {
            return AVIF_RESULT_NO_CONTENT;
        }
        colorTrack = &decoder->data->tracks.track[colorTrackIndex];

        colorProperties = avifSampleTableGetProperties(colorTrack->sampleTable);
        if (!colorProperties) {
            return AVIF_RESULT_BMFF_PARSE_FAILED;
        }

        // Find Exif and/or XMP metadata, if any
        if (colorTrack->meta) {
            // See the comment above avifDecoderDataFindMetadata() for the explanation of using 0 here
            if (!avifDecoderDataFindMetadata(data, colorTrack->meta, decoder->image, 0)) {
                return AVIF_RESULT_BMFF_PARSE_FAILED;
            }
        }

        uint32_t alphaTrackIndex = 0;
        for (; alphaTrackIndex < decoder->data->tracks.count; ++alphaTrackIndex) {
            avifTrack * track = &decoder->data->tracks.track[alphaTrackIndex];
            if (!track->sampleTable) {
                continue;
            }
            if (!track->id) {
                continue;
            }
            if (!track->sampleTable->chunks.count) {
                continue;
            }
            if (!avifSampleTableHasFormat(track->sampleTable, "av01")) {
                continue;
            }
            if (track->auxForID == colorTrack->id) {
                // Found it!
                break;
            }
        }
        if (alphaTrackIndex != decoder->data->tracks.count) {
            alphaTrack = &decoder->data->tracks.track[alphaTrackIndex];
        }

        avifTile * colorTile = avifDecoderDataCreateTile(decoder->data);
        if (!avifCodecDecodeInputGetSamples(colorTile->input, colorTrack->sampleTable, &decoder->data->rawInput)) {
            return AVIF_RESULT_BMFF_PARSE_FAILED;
        }
        decoder->data->colorTileCount = 1;

        avifTile * alphaTile = NULL;
        if (alphaTrack) {
            alphaTile = avifDecoderDataCreateTile(decoder->data);
            if (!avifCodecDecodeInputGetSamples(alphaTile->input, alphaTrack->sampleTable, &decoder->data->rawInput)) {
                return AVIF_RESULT_BMFF_PARSE_FAILED;
            }
            alphaTile->input->alpha = AVIF_TRUE;
            decoder->data->alphaTileCount = 1;
        }

        // Stash off sample table for future timing information
        data->sourceSampleTable = colorTrack->sampleTable;

        // Image sequence timing
        decoder->imageIndex = -1;
        decoder->imageCount = colorTile->input->samples.count;
        decoder->timescale = colorTrack->mediaTimescale;
        decoder->durationInTimescales = colorTrack->mediaDuration;
        if (colorTrack->mediaTimescale) {
            decoder->duration = (double)decoder->durationInTimescales / (double)colorTrack->mediaTimescale;
        } else {
            decoder->duration = 0;
        }
        memset(&decoder->imageTiming, 0, sizeof(decoder->imageTiming)); // to be set in avifDecoderNextImage()

        decoder->image->width = colorTrack->width;
        decoder->image->height = colorTrack->height;
        decoder->alphaPresent = (alphaTrack != NULL);
    } else {
        // Create from items

        avifROData colorOBU = AVIF_DATA_EMPTY;
        avifROData alphaOBU = AVIF_DATA_EMPTY;
        avifDecoderItem * colorOBUItem = NULL;
        avifDecoderItem * alphaOBUItem = NULL;

        // Find the colorOBU (primary) item
        for (uint32_t itemIndex = 0; itemIndex < data->meta->items.count; ++itemIndex) {
            avifDecoderItem * item = &data->meta->items.item[itemIndex];
            if (!item->size) {
                continue;
            }
            if (item->hasUnsupportedEssentialProperty) {
                // An essential property isn't supported by libavif; ignore the item.
                continue;
            }
            avifBool isGrid = (memcmp(item->type, "grid", 4) == 0);
            if (memcmp(item->type, "av01", 4) && !isGrid) {
                // probably exif or some other data
                continue;
            }
            if (item->thumbnailForID != 0) {
                // It's a thumbnail, skip it
                continue;
            }
            if ((data->meta->primaryItemID > 0) && (item->id != data->meta->primaryItemID)) {
                // a primary item ID was specified, require it
                continue;
            }

            if (isGrid) {
                const uint8_t * itemPtr = avifDecoderDataCalcItemPtr(data, item);
                if (itemPtr == NULL) {
                    return AVIF_RESULT_BMFF_PARSE_FAILED;
                }
                if (!avifParseImageGridBox(&data->colorGrid, itemPtr, item->size)) {
                    return AVIF_RESULT_INVALID_IMAGE_GRID;
                }
            } else {
                colorOBU.data = avifDecoderDataCalcItemPtr(data, item);
                colorOBU.size = item->size;
            }

            colorOBUItem = item;
            break;
        }

        if (!colorOBUItem) {
            return AVIF_RESULT_NO_AV1_ITEMS_FOUND;
        }
        colorProperties = &colorOBUItem->properties;

        // Find the alphaOBU item, if any
        for (uint32_t itemIndex = 0; itemIndex < data->meta->items.count; ++itemIndex) {
            avifDecoderItem * item = &data->meta->items.item[itemIndex];
            if (!item->size) {
                continue;
            }
            if (item->hasUnsupportedEssentialProperty) {
                // An essential property isn't supported by libavif; ignore the item.
                continue;
            }
            avifBool isGrid = (memcmp(item->type, "grid", 4) == 0);
            if (memcmp(item->type, "av01", 4) && !isGrid) {
                // probably exif or some other data
                continue;
            }
            if (item->thumbnailForID != 0) {
                // It's a thumbnail, skip it
                continue;
            }

            const avifProperty * auxCProp = avifPropertyArrayFind(&item->properties, "auxC");
            if (auxCProp && isAlphaURN(auxCProp->u.auxC.auxType) && (item->auxForID == colorOBUItem->id)) {
                if (isGrid) {
                    const uint8_t * itemPtr = avifDecoderDataCalcItemPtr(data, item);
                    if (itemPtr == NULL) {
                        return AVIF_RESULT_BMFF_PARSE_FAILED;
                    }
                    if (!avifParseImageGridBox(&data->alphaGrid, itemPtr, item->size)) {
                        return AVIF_RESULT_INVALID_IMAGE_GRID;
                    }
                } else {
                    alphaOBU.data = avifDecoderDataCalcItemPtr(data, item);
                    alphaOBU.size = item->size;
                }

                alphaOBUItem = item;
                break;
            }
        }

        // Find Exif and/or XMP metadata, if any
        if (!avifDecoderDataFindMetadata(data, data->meta, decoder->image, colorOBUItem->id)) {
            return AVIF_RESULT_BMFF_PARSE_FAILED;
        }

        if ((data->colorGrid.rows > 0) && (data->colorGrid.columns > 0)) {
            if (!avifDecoderDataGenerateImageGridTiles(data, &data->colorGrid, colorOBUItem, AVIF_FALSE)) {
                return AVIF_RESULT_INVALID_IMAGE_GRID;
            }
            data->colorTileCount = data->tiles.count;
        } else {
            if (colorOBU.size == 0) {
                return AVIF_RESULT_NO_AV1_ITEMS_FOUND;
            }

            avifTile * colorTile = avifDecoderDataCreateTile(decoder->data);
            avifDecodeSample * colorSample = (avifDecodeSample *)avifArrayPushPtr(&colorTile->input->samples);
            memcpy(&colorSample->data, &colorOBU, sizeof(avifROData));
            colorSample->sync = AVIF_TRUE;
            decoder->data->colorTileCount = 1;
        }

        if ((data->alphaGrid.rows > 0) && (data->alphaGrid.columns > 0) && alphaOBUItem) {
            if (!avifDecoderDataGenerateImageGridTiles(data, &data->alphaGrid, alphaOBUItem, AVIF_FALSE)) {
                return AVIF_RESULT_INVALID_IMAGE_GRID;
            }
            data->alphaTileCount = data->tiles.count - data->colorTileCount;
        } else {
            avifTile * alphaTile = NULL;
            if (alphaOBU.size > 0) {
                alphaTile = avifDecoderDataCreateTile(decoder->data);

                avifDecodeSample * alphaSample = (avifDecodeSample *)avifArrayPushPtr(&alphaTile->input->samples);
                memcpy(&alphaSample->data, &alphaOBU, sizeof(avifROData));
                alphaSample->sync = AVIF_TRUE;
                alphaTile->input->alpha = AVIF_TRUE;
                decoder->data->alphaTileCount = 1;
            }
        }

        // Set all counts and timing to safe-but-uninteresting values
        decoder->imageIndex = -1;
        decoder->imageCount = 1;
        decoder->imageTiming.timescale = 1;
        decoder->imageTiming.pts = 0;
        decoder->imageTiming.ptsInTimescales = 0;
        decoder->imageTiming.duration = 1;
        decoder->imageTiming.durationInTimescales = 1;
        decoder->timescale = 1;
        decoder->duration = 1;
        decoder->durationInTimescales = 1;

        decoder->ioStats.colorOBUSize = colorOBU.size;
        decoder->ioStats.alphaOBUSize = alphaOBU.size;

        const avifProperty * ispeProp = avifPropertyArrayFind(colorProperties, "ispe");
        if (ispeProp) {
            decoder->image->width = ispeProp->u.ispe.width;
            decoder->image->height = ispeProp->u.ispe.height;
        } else {
            decoder->image->width = 0;
            decoder->image->height = 0;
        }
        decoder->alphaPresent = (alphaOBUItem != NULL);
    }

    // Sanity check tiles
    for (uint32_t tileIndex = 0; tileIndex < data->tiles.count; ++tileIndex) {
        avifTile * tile = &data->tiles.tile[tileIndex];
        for (uint32_t sampleIndex = 0; sampleIndex < tile->input->samples.count; ++sampleIndex) {
            avifDecodeSample * sample = &tile->input->samples.sample[sampleIndex];
            if (!sample->data.data || !sample->data.size) {
                // Every sample must have some data
                return AVIF_RESULT_BMFF_PARSE_FAILED;
            }
        }
    }

    const avifProperty * colrProp = avifPropertyArrayFind(colorProperties, "colr");
    if (colrProp) {
        if (colrProp->u.colr.hasICC) {
            avifImageSetProfileICC(decoder->image, colrProp->u.colr.icc, colrProp->u.colr.iccSize);
        } else if (colrProp->u.colr.hasNCLX) {
            data->cicpSet = AVIF_TRUE;
            decoder->image->colorPrimaries = colrProp->u.colr.colorPrimaries;
            decoder->image->transferCharacteristics = colrProp->u.colr.transferCharacteristics;
            decoder->image->matrixCoefficients = colrProp->u.colr.matrixCoefficients;
            decoder->image->yuvRange = colrProp->u.colr.range;
        }
    }

    // Transformations
    const avifProperty * paspProp = avifPropertyArrayFind(colorProperties, "pasp");
    if (paspProp) {
        decoder->image->transformFlags |= AVIF_TRANSFORM_PASP;
        memcpy(&decoder->image->pasp, &paspProp->u.pasp, sizeof(avifPixelAspectRatioBox));
    }
    const avifProperty * clapProp = avifPropertyArrayFind(colorProperties, "clap");
    if (clapProp) {
        decoder->image->transformFlags |= AVIF_TRANSFORM_CLAP;
        memcpy(&decoder->image->clap, &clapProp->u.clap, sizeof(avifCleanApertureBox));
    }
    const avifProperty * irotProp = avifPropertyArrayFind(colorProperties, "irot");
    if (irotProp) {
        decoder->image->transformFlags |= AVIF_TRANSFORM_IROT;
        memcpy(&decoder->image->irot, &irotProp->u.irot, sizeof(avifImageRotation));
    }
    const avifProperty * imirProp = avifPropertyArrayFind(colorProperties, "imir");
    if (imirProp) {
        decoder->image->transformFlags |= AVIF_TRANSFORM_IMIR;
        memcpy(&decoder->image->imir, &imirProp->u.imir, sizeof(avifImageMirror));
    }

    if (!decoder->data->cicpSet && (data->tiles.count > 0)) {
        avifTile * firstTile = &data->tiles.tile[0];
        if (firstTile->input->samples.count > 0) {
            avifDecodeSample * sample = &firstTile->input->samples.sample[0];
            avifSequenceHeader sequenceHeader;
            if (avifSequenceHeaderParse(&sequenceHeader, &sample->data)) {
                decoder->data->cicpSet = AVIF_TRUE;
                decoder->image->colorPrimaries = sequenceHeader.colorPrimaries;
                decoder->image->transferCharacteristics = sequenceHeader.transferCharacteristics;
                decoder->image->matrixCoefficients = sequenceHeader.matrixCoefficients;
                decoder->image->yuvRange = sequenceHeader.range;
            }
        }
    }

    const avifProperty * av1CProp = avifPropertyArrayFind(colorProperties, "av1C");
    if (av1CProp) {
        decoder->image->depth = avifCodecConfigurationBoxGetDepth(&av1CProp->u.av1C);
        if (av1CProp->u.av1C.monochrome) {
            decoder->image->yuvFormat = AVIF_PIXEL_FORMAT_YUV400;
        } else {
            if (av1CProp->u.av1C.chromaSubsamplingX && av1CProp->u.av1C.chromaSubsamplingY) {
                decoder->image->yuvFormat = AVIF_PIXEL_FORMAT_YUV420;
            } else if (av1CProp->u.av1C.chromaSubsamplingX) {
                decoder->image->yuvFormat = AVIF_PIXEL_FORMAT_YUV422;

            } else {
                decoder->image->yuvFormat = AVIF_PIXEL_FORMAT_YUV444;
            }
        }
        decoder->image->yuvChromaSamplePosition = (avifChromaSamplePosition)av1CProp->u.av1C.chromaSamplePosition;
    } else {
        // An av1C box is mandatory in all valid AVIF configurations. Bail out.
        return AVIF_RESULT_BMFF_PARSE_FAILED;
    }

    return avifDecoderFlush(decoder);
}