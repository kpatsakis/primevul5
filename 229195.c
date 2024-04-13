void ntlm_get_version_info(NTLM_VERSION_INFO* versionInfo)
{
	OSVERSIONINFOA osVersionInfo;
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	GetVersionExA(&osVersionInfo);
	versionInfo->ProductMajorVersion = (UINT8)osVersionInfo.dwMajorVersion;
	versionInfo->ProductMinorVersion = (UINT8)osVersionInfo.dwMinorVersion;
	versionInfo->ProductBuild = (UINT16)osVersionInfo.dwBuildNumber;
	ZeroMemory(versionInfo->Reserved, sizeof(versionInfo->Reserved));
	versionInfo->NTLMRevisionCurrent = NTLMSSP_REVISION_W2K3;
}