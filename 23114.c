compileMacro(FileInfo *file, const Macro **macro) {

	// parse name
	CharsString token;
	if (!getToken(file, &token, "macro name")) return 0;
	switch (getOpcode(file, &token)) {
	case CTO_UpLow:	 // deprecated so "uplow" may be used as macro name
	case CTO_None:
		break;
	default:
		compileError(file, "Invalid macro name: already taken by an opcode");
		return 0;
	}
	for (int i = 0; i < token.length; i++) {
		if (!((token.chars[i] >= 'a' && token.chars[i] <= 'z') ||
					(token.chars[i] >= 'A' && token.chars[i] <= 'Z') ||
					(token.chars[i] >= '0' && token.chars[i] <= '9'))) {
			compileError(file,
					"Invalid macro name: must be a word containing only letters and "
					"digits");
			return 0;
		}
	}
	static char name[MAXSTRING + 1];
	int name_length;
	for (name_length = 0; name_length < token.length;
			name_length++)	// we know token can not be longer than MAXSTRING
		name[name_length] = (char)token.chars[name_length];
	name[name_length] = '\0';

	// parse body
	static widechar definition[MAXSTRING];
	static int substitutions[2 * MAX_MACRO_VAR];
	int definition_length = 0;
	int substitution_count = 0;
	int argument_count = 0;
	int dollar = 0;

	// ignore rest of line after name and read lines until "eom" is encountered
	while (_lou_getALine(file)) {
		if (file->linelen >= 3 && file->line[0] == 'e' && file->line[1] == 'o' &&
				file->line[2] == 'm') {
			*macro = create_macro(name, definition, definition_length, substitutions,
					substitution_count, argument_count);
			return 1;
		}
		while (!atEndOfLine(file)) {
			widechar c = file->line[file->linepos++];
			if (dollar) {
				dollar = 0;
				if (c >= '0' && c <= '9') {
					if (substitution_count >= MAX_MACRO_VAR) {
						compileError(file,
								"Macro can not have more than %d variable substitutions",
								MAXSTRING);
						return 0;
					}
					substitutions[2 * substitution_count] = definition_length;
					int arg = c - '0';
					substitutions[2 * substitution_count + 1] = arg;
					if (arg > argument_count) argument_count = arg;
					substitution_count++;
					continue;
				}
			} else if (c == '$') {
				dollar = 1;
				continue;
			}
			if (definition_length >= MAXSTRING) {
				compileError(file, "Macro exceeds %d characters", MAXSTRING);
				return 0;
			} else
				definition[definition_length++] = c;
		}
		dollar = 0;
		if (definition_length >= MAXSTRING) {
			compileError(file, "Macro exceeds %d characters", MAXSTRING);
			return 0;
		}
		definition[definition_length++] = '\n';
	}
	compileError(file, "macro must be terminated with 'eom'");
	return 0;
}