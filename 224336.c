static const avifPropertyArray * avifSampleTableGetProperties(const avifSampleTable * sampleTable)
{
    for (uint32_t i = 0; i < sampleTable->sampleDescriptions.count; ++i) {
        const avifSampleDescription * description = &sampleTable->sampleDescriptions.description[i];
        if (!memcmp(description->format, "av01", 4)) {
            return &description->properties;
        }
    }
    return NULL;
}