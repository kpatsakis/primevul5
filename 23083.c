parseDots(const FileInfo *file, CharsString *cells, const CharsString *token) {
	/* get dot patterns */
	widechar cell = 0; /* assembly place for dots */
	int cellCount = 0;
	int index;
	int start = 0;

	for (index = 0; index < token->length; index++) {
		int started = index != start;
		widechar character = token->chars[index];
		switch (character) { /* or dots to make up Braille cell */
			{
				int dot;
			case '1':
				dot = LOU_DOT_1;
				goto haveDot;
			case '2':
				dot = LOU_DOT_2;
				goto haveDot;
			case '3':
				dot = LOU_DOT_3;
				goto haveDot;
			case '4':
				dot = LOU_DOT_4;
				goto haveDot;
			case '5':
				dot = LOU_DOT_5;
				goto haveDot;
			case '6':
				dot = LOU_DOT_6;
				goto haveDot;
			case '7':
				dot = LOU_DOT_7;
				goto haveDot;
			case '8':
				dot = LOU_DOT_8;
				goto haveDot;
			case '9':
				dot = LOU_DOT_9;
				goto haveDot;
			case 'a':
			case 'A':
				dot = LOU_DOT_10;
				goto haveDot;
			case 'b':
			case 'B':
				dot = LOU_DOT_11;
				goto haveDot;
			case 'c':
			case 'C':
				dot = LOU_DOT_12;
				goto haveDot;
			case 'd':
			case 'D':
				dot = LOU_DOT_13;
				goto haveDot;
			case 'e':
			case 'E':
				dot = LOU_DOT_14;
				goto haveDot;
			case 'f':
			case 'F':
				dot = LOU_DOT_15;
			haveDot:
				if (started && !cell) goto invalid;
				if (cell & dot) {
					compileError(file, "dot specified more than once.");
					return 0;
				}
				cell |= dot;
				break;
			}
		case '0': /* blank */
			if (started) goto invalid;
			break;
		case '-': /* got all dots for this cell */
			if (!started) {
				compileError(file, "missing cell specification.");
				return 0;
			}
			cells->chars[cellCount++] = cell | LOU_DOTS;
			cell = 0;
			start = index + 1;
			break;
		default:
		invalid:
			compileError(
					file, "invalid dot number %s.", _lou_showString(&character, 1, 0));
			return 0;
		}
	}
	if (index == start) {
		compileError(file, "missing cell specification.");
		return 0;
	}
	cells->chars[cellCount++] = cell | LOU_DOTS; /* last cell */
	cells->length = cellCount;
	return 1;
}