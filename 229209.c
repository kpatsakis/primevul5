void ntlm_print_version_info(NTLM_VERSION_INFO* versionInfo)
{
	WLog_INFO(TAG, "VERSION ={");
	WLog_INFO(TAG, "\tProductMajorVersion: %" PRIu8 "", versionInfo->ProductMajorVersion);
	WLog_INFO(TAG, "\tProductMinorVersion: %" PRIu8 "", versionInfo->ProductMinorVersion);
	WLog_INFO(TAG, "\tProductBuild: %" PRIu16 "", versionInfo->ProductBuild);
	WLog_INFO(TAG, "\tReserved: 0x%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "", versionInfo->Reserved[0],
	          versionInfo->Reserved[1], versionInfo->Reserved[2]);
	WLog_INFO(TAG, "\tNTLMRevisionCurrent: 0x%02" PRIX8 "", versionInfo->NTLMRevisionCurrent);
}