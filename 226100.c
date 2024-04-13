tiff_rgb_print_page(gx_device_printer * pdev, gp_file * file)
{
    gx_device_tiff *const tfdev = (gx_device_tiff *)pdev;
    int code;

    /* open the TIFF device */
    if (gdev_prn_file_is_new(pdev)) {
        tfdev->tif = tiff_from_filep(pdev, pdev->dname, file, tfdev->BigEndian, tfdev->UseBigTIFF);
        if (!tfdev->tif)
            return_error(gs_error_invalidfileaccess);
    }

    code = gdev_tiff_begin_page(tfdev, file);
    if (code < 0)
        return code;

    TIFFSetField(tfdev->tif, TIFFTAG_BITSPERSAMPLE,
                 pdev->color_info.depth / pdev->color_info.num_components);
    tiff_set_rgb_fields(tfdev);

    /* Write the page data. */
    return tiff_print_page(pdev, tfdev->tif, 0);
}