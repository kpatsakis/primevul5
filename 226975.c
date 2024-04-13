LICENSE_PRODUCT_INFO* license_new_product_info()
{
	LICENSE_PRODUCT_INFO* productInfo;
	productInfo = (LICENSE_PRODUCT_INFO*)malloc(sizeof(LICENSE_PRODUCT_INFO));
	if (!productInfo)
		return NULL;
	productInfo->dwVersion = 0;
	productInfo->cbCompanyName = 0;
	productInfo->pbCompanyName = NULL;
	productInfo->cbProductId = 0;
	productInfo->pbProductId = NULL;
	return productInfo;
}