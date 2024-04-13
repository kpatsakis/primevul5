static avifBool avifParseColourInformationBox(avifProperty * prop, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    avifColourInformationBox * colr = &prop->u.colr;
    colr->hasICC = AVIF_FALSE;
    colr->hasNCLX = AVIF_FALSE;

    uint8_t colorType[4]; // unsigned int(32) colour_type;
    CHECK(avifROStreamRead(&s, colorType, 4));
    if (!memcmp(colorType, "rICC", 4) || !memcmp(colorType, "prof", 4)) {
        colr->hasICC = AVIF_TRUE;
        colr->icc = avifROStreamCurrent(&s);
        colr->iccSize = avifROStreamRemainingBytes(&s);
    } else if (!memcmp(colorType, "nclx", 4)) {
        uint16_t tmp16;
        // unsigned int(16) colour_primaries;
        CHECK(avifROStreamReadU16(&s, &tmp16));
        colr->colorPrimaries = (avifColorPrimaries)tmp16;
        // unsigned int(16) transfer_characteristics;
        CHECK(avifROStreamReadU16(&s, &tmp16));
        colr->transferCharacteristics = (avifTransferCharacteristics)tmp16;
        // unsigned int(16) matrix_coefficients;
        CHECK(avifROStreamReadU16(&s, &tmp16));
        colr->matrixCoefficients = (avifMatrixCoefficients)tmp16;
        // unsigned int(1) full_range_flag;
        // unsigned int(7) reserved = 0;
        uint8_t tmp8;
        CHECK(avifROStreamRead(&s, &tmp8, 1));
        colr->range = (tmp8 & 0x80) ? AVIF_RANGE_FULL : AVIF_RANGE_LIMITED;
        colr->hasNCLX = AVIF_TRUE;
    }
    return AVIF_TRUE;
}