void license_free_product_info(LICENSE_PRODUCT_INFO* productInfo)
{
	if (productInfo)
	{
		free(productInfo->pbCompanyName);
		free(productInfo->pbProductId);
		free(productInfo);
	}
}