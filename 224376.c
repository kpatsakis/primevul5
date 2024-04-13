static avifBool avifFileTypeIsCompatible(avifFileType * ftyp)
{
    avifBool avifCompatible = (memcmp(ftyp->majorBrand, "avif", 4) == 0 || memcmp(ftyp->majorBrand, "avis", 4) == 0);
    if (!avifCompatible) {
        for (int compatibleBrandIndex = 0; compatibleBrandIndex < ftyp->compatibleBrandsCount; ++compatibleBrandIndex) {
            const uint8_t * compatibleBrand = &ftyp->compatibleBrands[4 * compatibleBrandIndex];
            if (!memcmp(compatibleBrand, "avif", 4) || !memcmp(compatibleBrand, "avis", 4)) {
                avifCompatible = AVIF_TRUE;
                break;
            }
        }
    }
    return avifCompatible;
}