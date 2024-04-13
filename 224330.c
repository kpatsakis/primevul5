static avifBool avifParseItemPropertyAssociation(avifMeta * meta, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    uint8_t version;
    uint32_t flags;
    CHECK(avifROStreamReadVersionAndFlags(&s, &version, &flags));
    avifBool propertyIndexIsU16 = ((flags & 0x1) != 0);

    uint32_t entryCount;
    CHECK(avifROStreamReadU32(&s, &entryCount));
    for (uint32_t entryIndex = 0; entryIndex < entryCount; ++entryIndex) {
        unsigned int itemID;
        if (version < 1) {
            uint16_t tmp;
            CHECK(avifROStreamReadU16(&s, &tmp));
            itemID = tmp;
        } else {
            CHECK(avifROStreamReadU32(&s, &itemID));
        }
        uint8_t associationCount;
        CHECK(avifROStreamRead(&s, &associationCount, 1));
        for (uint8_t associationIndex = 0; associationIndex < associationCount; ++associationIndex) {
            avifBool essential = AVIF_FALSE;
            uint16_t propertyIndex = 0;
            if (propertyIndexIsU16) {
                CHECK(avifROStreamReadU16(&s, &propertyIndex));
                essential = ((propertyIndex & 0x8000) != 0);
                propertyIndex &= 0x7fff;
            } else {
                uint8_t tmp;
                CHECK(avifROStreamRead(&s, &tmp, 1));
                essential = ((tmp & 0x80) != 0);
                propertyIndex = tmp & 0x7f;
            }

            if (propertyIndex == 0) {
                // Not associated with any item
                continue;
            }
            --propertyIndex; // 1-indexed

            if (propertyIndex >= meta->properties.count) {
                return AVIF_FALSE;
            }

            avifDecoderItem * item = avifMetaFindItem(meta, itemID);
            if (!item) {
                return AVIF_FALSE;
            }

            // Copy property to item
            avifProperty * srcProp = &meta->properties.prop[propertyIndex];

            static const char * supportedTypes[] = { "ispe", "auxC", "colr", "av1C", "pasp", "clap", "irot", "imir", "pixi" };
            size_t supportedTypesCount = sizeof(supportedTypes) / sizeof(supportedTypes[0]);
            avifBool supportedType = AVIF_FALSE;
            for (size_t i = 0; i < supportedTypesCount; ++i) {
                if (!memcmp(srcProp->type, supportedTypes[i], 4)) {
                    supportedType = AVIF_TRUE;
                    break;
                }
            }
            if (supportedType) {
                avifProperty * dstProp = (avifProperty *)avifArrayPushPtr(&item->properties);
                memcpy(dstProp, srcProp, sizeof(avifProperty));
            } else {
                if (essential) {
                    // Discovered an essential item property that libavif doesn't support!
                    // Make a note to ignore this item later.
                    item->hasUnsupportedEssentialProperty = AVIF_TRUE;
                }
            }
        }
    }

    return AVIF_TRUE;
}