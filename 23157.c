passGetName(CharsString *passLine, int *passLinepos, CharsString *name) {
	name->length = 0;
	// a name is a sequence of characters in the ranges 'a'..'z' and 'A'..'Z'
	do {
		widechar c = passLine->chars[*passLinepos];
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
			name->chars[name->length++] = c;
			(*passLinepos)++;
		} else {
			break;
		}
	} while (*passLinepos < passLine->length);
	return 1;
}