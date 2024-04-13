tiff_set_rgb_fields(gx_device_tiff *tfdev)
{
    /* Put in a switch statement in case we want to have others */
    switch (tfdev->icc_struct->device_profile[0]->data_cs) {
        case gsRGB:
            TIFFSetField(tfdev->tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
            break;
        case gsCIELAB:
            TIFFSetField(tfdev->tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_ICCLAB);
            break;
        default:
            TIFFSetField(tfdev->tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
            break;
    }
    TIFFSetField(tfdev->tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tfdev->tif, TIFFTAG_SAMPLESPERPIXEL, 3);

    tiff_set_compression((gx_device_printer *)tfdev, tfdev->tif,
                         tfdev->Compression, tfdev->MaxStripSize);
}