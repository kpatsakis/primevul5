static avifBool avifParseAV1CodecConfigurationBoxProperty(avifProperty * prop, const uint8_t * raw, size_t rawLen)
{
    return avifParseAV1CodecConfigurationBox(raw, rawLen, &prop->u.av1C);
}