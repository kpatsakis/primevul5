getOpcode(const FileInfo *file, const CharsString *token) {
	static TranslationTableOpcode lastOpcode = 0;
	TranslationTableOpcode opcode = lastOpcode;

	do {
		if (token->length == opcodeLengths[opcode])
			if (eqasc2uni((unsigned char *)opcodeNames[opcode], &token->chars[0],
						token->length)) {
				lastOpcode = opcode;
				return opcode;
			}
		opcode++;
		if (opcode >= CTO_None) opcode = 0;
	} while (opcode != lastOpcode);
	return CTO_None;
}