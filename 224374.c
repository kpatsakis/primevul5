static avifBool avifParseItemLocationBox(avifMeta * meta, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    uint8_t version;
    CHECK(avifROStreamReadVersionAndFlags(&s, &version, NULL));
    if (version > 2) {
        return AVIF_FALSE;
    }

    uint8_t offsetSizeAndLengthSize;
    CHECK(avifROStreamRead(&s, &offsetSizeAndLengthSize, 1));
    uint8_t offsetSize = (offsetSizeAndLengthSize >> 4) & 0xf; // unsigned int(4) offset_size;
    uint8_t lengthSize = (offsetSizeAndLengthSize >> 0) & 0xf; // unsigned int(4) length_size;

    uint8_t baseOffsetSizeAndIndexSize;
    CHECK(avifROStreamRead(&s, &baseOffsetSizeAndIndexSize, 1));
    uint8_t baseOffsetSize = (baseOffsetSizeAndIndexSize >> 4) & 0xf; // unsigned int(4) base_offset_size;
    uint8_t indexSize = 0;
    if ((version == 1) || (version == 2)) {
        indexSize = baseOffsetSizeAndIndexSize & 0xf; // unsigned int(4) index_size;
        if (indexSize != 0) {
            // extent_index unsupported
            return AVIF_FALSE;
        }
    }

    uint16_t tmp16;
    uint32_t itemCount;
    if (version < 2) {
        CHECK(avifROStreamReadU16(&s, &tmp16)); // unsigned int(16) item_count;
        itemCount = tmp16;
    } else {
        CHECK(avifROStreamReadU32(&s, &itemCount)); // unsigned int(32) item_count;
    }
    for (uint32_t i = 0; i < itemCount; ++i) {
        uint32_t itemID;
        uint32_t idatID = 0;
        if (version < 2) {
            CHECK(avifROStreamReadU16(&s, &tmp16)); // unsigned int(16) item_ID;
            itemID = tmp16;
        } else {
            CHECK(avifROStreamReadU32(&s, &itemID)); // unsigned int(32) item_ID;
        }

        if ((version == 1) || (version == 2)) {
            uint8_t ignored;
            uint8_t constructionMethod;
            CHECK(avifROStreamRead(&s, &ignored, 1));            // unsigned int(12) reserved = 0;
            CHECK(avifROStreamRead(&s, &constructionMethod, 1)); // unsigned int(4) construction_method;
            constructionMethod = constructionMethod & 0xf;
            if ((constructionMethod != 0 /* file */) && (constructionMethod != 1 /* idat */)) {
                // construction method item(2) unsupported
                return AVIF_FALSE;
            }
            if (constructionMethod == 1) {
                idatID = meta->idatID;
            }
        }

        uint16_t dataReferenceIndex;                                 // unsigned int(16) data_ref rence_index;
        CHECK(avifROStreamReadU16(&s, &dataReferenceIndex));         //
        uint64_t baseOffset;                                         // unsigned int(base_offset_size*8) base_offset;
        CHECK(avifROStreamReadUX8(&s, &baseOffset, baseOffsetSize)); //
        uint16_t extentCount;                                        // unsigned int(16) extent_count;
        CHECK(avifROStreamReadU16(&s, &extentCount));                //
        if (extentCount == 1) {
            // If extent_index is ever supported, this spec must be implemented here:
            // ::  if (((version == 1) || (version == 2)) && (index_size > 0)) {
            // ::      unsigned int(index_size*8) extent_index;
            // ::  }

            uint64_t extentOffset; // unsigned int(offset_size*8) extent_offset;
            CHECK(avifROStreamReadUX8(&s, &extentOffset, offsetSize));
            uint64_t extentLength; // unsigned int(offset_size*8) extent_length;
            CHECK(avifROStreamReadUX8(&s, &extentLength, lengthSize));

            avifDecoderItem * item = avifMetaFindItem(meta, itemID);
            if (!item) {
                return AVIF_FALSE;
            }
            item->id = itemID;
            item->offset = (uint32_t)(baseOffset + extentOffset);
            item->size = (uint32_t)extentLength;
            item->idatID = idatID;
        } else {
            // TODO: support more than one extent
            return AVIF_FALSE;
        }
    }
    return AVIF_TRUE;
}