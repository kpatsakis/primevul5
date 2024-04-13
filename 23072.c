_lou_getCharForDots(widechar d, const DisplayTableHeader *table) {
	CharDotsMapping *cdPtr = getCharForDots(d, table);
	if (cdPtr) return cdPtr->found;
	return '\0';
}