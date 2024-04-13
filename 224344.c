static avifBool avifSampleTableHasFormat(const avifSampleTable * sampleTable, const char * format)
{
    for (uint32_t i = 0; i < sampleTable->sampleDescriptions.count; ++i) {
        if (!memcmp(sampleTable->sampleDescriptions.description[i].format, format, 4)) {
            return AVIF_TRUE;
        }
    }
    return AVIF_FALSE;
}