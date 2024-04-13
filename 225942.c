static inline struct device *ms_dev(struct rtsx_usb_ms *host)
{
	return &(host->pdev->dev);
}