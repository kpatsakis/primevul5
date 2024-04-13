bool Http::CheckIfMatch(LPMAPIPROP lpProp)
{
	bool ret = false, invert = false;
	std::string strIf, strValue;
	memory_ptr<SPropValue> ptrLastModTime;

	if (lpProp != nullptr &&
	    HrGetOneProp(lpProp, PR_LAST_MODIFICATION_TIME, &~ptrLastModTime) == hrSuccess)
		strValue = stringify_int64(FileTimeToUnixTime(ptrLastModTime->Value.ft), false);

	if (HrGetHeaderValue("If-Match", &strIf) == hrSuccess) {
		if (strIf == "*" && ptrLastModTime == nullptr)
			// we have an object without a last mod time, not allowed
			return false;
	} else if (HrGetHeaderValue("If-None-Match", &strIf) == hrSuccess) {
		if (strIf == "*" && ptrLastModTime != nullptr)
			// we have an object which has a last mod time, not allowed
			return false;
		invert = true;
	} else {
		return true;
	}

	// check all etags for a match
	for (auto &i : tokenize(strIf, ',', true)) {
		if (i.at(0) == '"' || i.at(0) == '\'')
			i.assign(i.begin() + 1, i.end() - 1);
		if (i == strValue) {
			ret = true;
			break;
		}
	}
	if (invert)
		ret = !ret;
	return ret;
}