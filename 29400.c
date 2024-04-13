zdoneshowpage(i_ctx_t *i_ctx_p)
{
    gx_device *dev = gs_currentdevice(igs);
    gx_device *tdev = (*dev_proc(dev, get_page_device)) (dev);

    if (tdev != 0)
        tdev->ShowpageCount++;
    return 0;
}
