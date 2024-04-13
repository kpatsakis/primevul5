static void license_print_product_info(const LICENSE_PRODUCT_INFO* productInfo)
{
	char* CompanyName = NULL;
	char* ProductId = NULL;
	ConvertFromUnicode(CP_UTF8, 0, (WCHAR*)productInfo->pbCompanyName,
	                   productInfo->cbCompanyName / 2, &CompanyName, 0, NULL, NULL);
	ConvertFromUnicode(CP_UTF8, 0, (WCHAR*)productInfo->pbProductId, productInfo->cbProductId / 2,
	                   &ProductId, 0, NULL, NULL);
	WLog_INFO(TAG, "ProductInfo:");
	WLog_INFO(TAG, "\tdwVersion: 0x%08" PRIX32 "", productInfo->dwVersion);
	WLog_INFO(TAG, "\tCompanyName: %s", CompanyName);
	WLog_INFO(TAG, "\tProductId: %s", ProductId);
	free(CompanyName);
	free(ProductId);
}