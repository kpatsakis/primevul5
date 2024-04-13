static avifBool avifDecoderDataFillImageGrid(avifDecoderData * data,
                                             avifImageGrid * grid,
                                             avifImage * dstImage,
                                             unsigned int firstTileIndex,
                                             unsigned int tileCount,
                                             avifBool alpha)
{
    if (tileCount == 0) {
        return AVIF_FALSE;
    }

    avifTile * firstTile = &data->tiles.tile[firstTileIndex];
    avifBool firstTileUVPresent = (firstTile->image->yuvPlanes[AVIF_CHAN_U] && firstTile->image->yuvPlanes[AVIF_CHAN_V]);

    // Check for tile consistency: All tiles in a grid image should match in the properties checked below.
    for (unsigned int i = 1; i < tileCount; ++i) {
        avifTile * tile = &data->tiles.tile[firstTileIndex + i];
        avifBool uvPresent = (tile->image->yuvPlanes[AVIF_CHAN_U] && tile->image->yuvPlanes[AVIF_CHAN_V]);
        if ((tile->image->width != firstTile->image->width) || (tile->image->height != firstTile->image->height) ||
            (tile->image->depth != firstTile->image->depth) || (tile->image->yuvFormat != firstTile->image->yuvFormat) ||
            (tile->image->yuvRange != firstTile->image->yuvRange) || (uvPresent != firstTileUVPresent) ||
            ((tile->image->colorPrimaries != firstTile->image->colorPrimaries) ||
             (tile->image->transferCharacteristics != firstTile->image->transferCharacteristics) ||
             (tile->image->matrixCoefficients != firstTile->image->matrixCoefficients))) {
            return AVIF_FALSE;
        }
    }

    // Lazily populate dstImage with the new frame's properties. If we're decoding alpha,
    // these values must already match.
    if ((dstImage->width != grid->outputWidth) || (dstImage->height != grid->outputHeight) ||
        (dstImage->depth != firstTile->image->depth) || (dstImage->yuvFormat != firstTile->image->yuvFormat)) {
        if (alpha) {
            // Alpha doesn't match size, just bail out
            return AVIF_FALSE;
        }

        avifImageFreePlanes(dstImage, AVIF_PLANES_ALL);
        dstImage->width = grid->outputWidth;
        dstImage->height = grid->outputHeight;
        dstImage->depth = firstTile->image->depth;
        dstImage->yuvFormat = firstTile->image->yuvFormat;
        dstImage->yuvRange = firstTile->image->yuvRange;
        if (!data->cicpSet) {
            data->cicpSet = AVIF_TRUE;
            dstImage->colorPrimaries = firstTile->image->colorPrimaries;
            dstImage->transferCharacteristics = firstTile->image->transferCharacteristics;
            dstImage->matrixCoefficients = firstTile->image->matrixCoefficients;
        }
    }
    if (alpha) {
        dstImage->alphaRange = firstTile->image->alphaRange;
    }

    avifImageAllocatePlanes(dstImage, alpha ? AVIF_PLANES_A : AVIF_PLANES_YUV);

    avifPixelFormatInfo formatInfo;
    avifGetPixelFormatInfo(firstTile->image->yuvFormat, &formatInfo);

    unsigned int tileIndex = firstTileIndex;
    size_t pixelBytes = avifImageUsesU16(dstImage) ? 2 : 1;
    for (unsigned int rowIndex = 0; rowIndex < grid->rows; ++rowIndex) {
        for (unsigned int colIndex = 0; colIndex < grid->columns; ++colIndex, ++tileIndex) {
            avifTile * tile = &data->tiles.tile[tileIndex];

            unsigned int widthToCopy = firstTile->image->width;
            unsigned int maxX = firstTile->image->width * (colIndex + 1);
            if (maxX > grid->outputWidth) {
                widthToCopy -= maxX - grid->outputWidth;
            }

            unsigned int heightToCopy = firstTile->image->height;
            unsigned int maxY = firstTile->image->height * (rowIndex + 1);
            if (maxY > grid->outputHeight) {
                heightToCopy -= maxY - grid->outputHeight;
            }

            // Y and A channels
            size_t yaColOffset = colIndex * firstTile->image->width;
            size_t yaRowOffset = rowIndex * firstTile->image->height;
            size_t yaRowBytes = widthToCopy * pixelBytes;

            if (alpha) {
                // A
                for (unsigned int j = 0; j < heightToCopy; ++j) {
                    uint8_t * src = &tile->image->alphaPlane[j * tile->image->alphaRowBytes];
                    uint8_t * dst = &dstImage->alphaPlane[(yaColOffset * pixelBytes) + ((yaRowOffset + j) * dstImage->alphaRowBytes)];
                    memcpy(dst, src, yaRowBytes);
                }
            } else {
                // Y
                for (unsigned int j = 0; j < heightToCopy; ++j) {
                    uint8_t * src = &tile->image->yuvPlanes[AVIF_CHAN_Y][j * tile->image->yuvRowBytes[AVIF_CHAN_Y]];
                    uint8_t * dst =
                        &dstImage->yuvPlanes[AVIF_CHAN_Y][(yaColOffset * pixelBytes) + ((yaRowOffset + j) * dstImage->yuvRowBytes[AVIF_CHAN_Y])];
                    memcpy(dst, src, yaRowBytes);
                }

                if (!firstTileUVPresent) {
                    continue;
                }

                // UV
                heightToCopy >>= formatInfo.chromaShiftY;
                size_t uvColOffset = yaColOffset >> formatInfo.chromaShiftX;
                size_t uvRowOffset = yaRowOffset >> formatInfo.chromaShiftY;
                size_t uvRowBytes = yaRowBytes >> formatInfo.chromaShiftX;
                for (unsigned int j = 0; j < heightToCopy; ++j) {
                    uint8_t * srcU = &tile->image->yuvPlanes[AVIF_CHAN_U][j * tile->image->yuvRowBytes[AVIF_CHAN_U]];
                    uint8_t * dstU =
                        &dstImage->yuvPlanes[AVIF_CHAN_U][(uvColOffset * pixelBytes) + ((uvRowOffset + j) * dstImage->yuvRowBytes[AVIF_CHAN_U])];
                    memcpy(dstU, srcU, uvRowBytes);

                    uint8_t * srcV = &tile->image->yuvPlanes[AVIF_CHAN_V][j * tile->image->yuvRowBytes[AVIF_CHAN_V]];
                    uint8_t * dstV =
                        &dstImage->yuvPlanes[AVIF_CHAN_V][(uvColOffset * pixelBytes) + ((uvRowOffset + j) * dstImage->yuvRowBytes[AVIF_CHAN_V])];
                    memcpy(dstV, srcV, uvRowBytes);
                }
            }
        }
    }

    return AVIF_TRUE;
}