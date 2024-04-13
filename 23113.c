_lou_getDotsForChar(widechar c, const DisplayTableHeader *table) {
	CharDotsMapping *cdPtr = getDotsForChar(c, table);
	if (cdPtr) return cdPtr->found;
	return LOU_DOTS;
}