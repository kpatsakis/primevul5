HRESULT Http::HrGetDepth(ULONG *ulDepth)
{
	std::string strDepth;
	/*
	 * Valid input: [0, 1, infinity]
	 */
	auto hr = HrGetHeaderValue("Depth", &strDepth);
	if (hr != hrSuccess)
		*ulDepth = 0; /* Default is no subfolders. Default should become a parameter. It is action dependent. */
	else if (strDepth == "infinity")
		*ulDepth = 2;
	else {
		*ulDepth = atoi(strDepth.c_str());
		if (*ulDepth > 1)
			*ulDepth = 1;
	}
	return hr;
}