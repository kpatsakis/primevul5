static void hostdev_unlink(USBHostDevice *dev)
{
    USBHostDevice *pdev = hostdev_list;
    USBHostDevice **prev = &hostdev_list;

    while (pdev) {
	if (pdev == dev) {
            *prev = dev->next;
            return;
        }

        prev = &pdev->next;
        pdev = pdev->next;
    }
}