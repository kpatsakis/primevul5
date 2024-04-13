static const uint8_t * avifDecoderDataCalcItemPtr(avifDecoderData * data, avifDecoderItem * item)
{
    avifROData * offsetBuffer = NULL;
    if (item->idatID == 0) {
        // construction_method: file(0)

        offsetBuffer = &data->rawInput;
    } else {
        // construction_method: idat(1)

        // Find associated idat block
        for (uint32_t i = 0; i < item->meta->idats.count; ++i) {
            if (item->meta->idats.idat[i].id == item->idatID) {
                offsetBuffer = &item->meta->idats.idat[i].data;
                break;
            }
        }

        if (offsetBuffer == NULL) {
            // no idat box was found in this meta box, bail out
            return NULL;
        }
    }

    if (item->offset > offsetBuffer->size) {
        return NULL;
    }
    uint64_t offsetSize = (uint64_t)item->offset + (uint64_t)item->size;
    if (offsetSize > (uint64_t)offsetBuffer->size) {
        return NULL;
    }
    return offsetBuffer->data + item->offset;
}