compilePassOpcode(const FileInfo *file, TranslationTableOpcode opcode, int noback,
		int nofor, TranslationTableHeader **table) {
	static CharsString passRuleChars;
	static CharsString passRuleDots;
	/* Compile the operands of a pass opcode */
	widechar passSubOp;
	const CharacterClass *class;
	TranslationTableRule *rule = NULL;
	int k;
	int kk = 0;
	int endTest = 0;
	widechar *passInstructions = passRuleDots.chars;
	int passIC = 0; /* Instruction counter */
	passRuleChars.length = 0;
	CharsString passHoldString;
	widechar passHoldNumber;
	CharsString passLine;
	int passLinepos = 0;
	TranslationTableCharacterAttributes passAttributes;
	passHoldString.length = 0;
	for (k = file->linepos; k < file->linelen; k++)
		passHoldString.chars[passHoldString.length++] = file->line[k];
#define SEPCHAR 0x0001
	for (k = 0; k < passHoldString.length && passHoldString.chars[k] > 32; k++)
		;
	if (k < passHoldString.length)
		passHoldString.chars[k] = SEPCHAR;
	else {
		compileError(file, "Invalid multipass operands");
		return 0;
	}
	parseChars(file, &passLine, &passHoldString);
	/* Compile test part */
	for (k = 0; k < passLine.length && passLine.chars[k] != SEPCHAR; k++)
		;
	endTest = k;
	passLine.chars[endTest] = pass_endTest;
	passLinepos = 0;
	while (passLinepos <= endTest) {
		switch ((passSubOp = passLine.chars[passLinepos])) {
		case pass_lookback:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_lookback))
				return 0;
			passLinepos++;
			passGetNumber(&passLine, &passLinepos, &passHoldNumber);
			if (passHoldNumber == 0) passHoldNumber = 1;
			if (!appendInstructionChar(file, passInstructions, &passIC, passHoldNumber))
				return 0;
			break;
		case pass_not:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_not))
				return 0;
			passLinepos++;
			break;
		case pass_first:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_first))
				return 0;
			passLinepos++;
			break;
		case pass_last:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_last))
				return 0;
			passLinepos++;
			break;
		case pass_search:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_search))
				return 0;
			passLinepos++;
			break;
		case pass_string:
			if (!verifyStringOrDots(file, opcode, 1, 0, nofor)) {
				return 0;
			}
			passLinepos++;
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_string))
				return 0;
			passGetString(&passLine, &passLinepos, &passHoldString, file);
			if (passHoldString.length == 0) {
				compileError(file, "empty string in test part");
				return 0;
			}
			goto testDoCharsDots;
		case pass_dots:
			if (!verifyStringOrDots(file, opcode, 0, 0, nofor)) {
				return 0;
			}
			passLinepos++;
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_dots))
				return 0;
			passGetDots(&passLine, &passLinepos, &passHoldString, file);
			if (passHoldString.length == 0) {
				compileError(file, "expected dot pattern after @ operand in test part");
				return 0;
			}
		testDoCharsDots:
			if (passIC >= MAXSTRING) {
				compileError(
						file, "@ operand in test part of multipass operand too long");
				return 0;
			}
			if (!appendInstructionChar(
						file, passInstructions, &passIC, passHoldString.length))
				return 0;
			for (kk = 0; kk < passHoldString.length; kk++) {
				if (passIC >= MAXSTRING) {
					compileError(
							file, "@ operand in test part of multipass operand too long");
					return 0;
				}
				if (!appendInstructionChar(
							file, passInstructions, &passIC, passHoldString.chars[kk]))
					return 0;
			}
			break;
		case pass_startReplace:
			if (!appendInstructionChar(
						file, passInstructions, &passIC, pass_startReplace))
				return 0;
			passLinepos++;
			break;
		case pass_endReplace:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_endReplace))
				return 0;
			passLinepos++;
			break;
		case pass_variable:
			passLinepos++;
			if (!passGetVariableNumber(file, &passLine, &passLinepos, &passHoldNumber))
				return 0;
			switch (passLine.chars[passLinepos]) {
			case pass_eq:
				if (!appendInstructionChar(file, passInstructions, &passIC, pass_eq))
					return 0;
				goto doComp;
			case pass_lt:
				if (passLine.chars[passLinepos + 1] == pass_eq) {
					passLinepos++;
					if (!appendInstructionChar(
								file, passInstructions, &passIC, pass_lteq))
						return 0;
				} else if (!appendInstructionChar(
								   file, passInstructions, &passIC, pass_lt))
					return 0;
				goto doComp;
			case pass_gt:
				if (passLine.chars[passLinepos + 1] == pass_eq) {
					passLinepos++;
					if (!appendInstructionChar(
								file, passInstructions, &passIC, pass_gteq))
						return 0;
				} else if (!appendInstructionChar(
								   file, passInstructions, &passIC, pass_gt))
					return 0;
			doComp:
				if (!appendInstructionChar(
							file, passInstructions, &passIC, passHoldNumber))
					return 0;
				passLinepos++;
				passGetNumber(&passLine, &passLinepos, &passHoldNumber);
				if (!appendInstructionChar(
							file, passInstructions, &passIC, passHoldNumber))
					return 0;
				break;
			default:
				compileError(file, "incorrect comparison operator");
				return 0;
			}
			break;
		case pass_attributes:
			passLinepos++;
			if (!passGetAttributes(&passLine, &passLinepos, &passAttributes, file))
				return 0;
		insertAttributes:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_attributes))
				return 0;
			if (!appendInstructionChar(
						file, passInstructions, &passIC, (passAttributes >> 48) & 0xffff))
				return 0;
			if (!appendInstructionChar(
						file, passInstructions, &passIC, (passAttributes >> 32) & 0xffff))
				return 0;
			if (!appendInstructionChar(
						file, passInstructions, &passIC, (passAttributes >> 16) & 0xffff))
				return 0;
			if (!appendInstructionChar(
						file, passInstructions, &passIC, passAttributes & 0xffff))
				return 0;
		getRange:
			if (passLine.chars[passLinepos] == pass_until) {
				passLinepos++;
				if (!appendInstructionChar(file, passInstructions, &passIC, 1)) return 0;
				if (!appendInstructionChar(file, passInstructions, &passIC, 0xffff))
					return 0;
				break;
			}
			passGetNumber(&passLine, &passLinepos, &passHoldNumber);
			if (passHoldNumber == 0) {
				if (!appendInstructionChar(file, passInstructions, &passIC, 1)) return 0;
				if (!appendInstructionChar(file, passInstructions, &passIC, 1)) return 0;
				break;
			}
			if (!appendInstructionChar(file, passInstructions, &passIC, passHoldNumber))
				return 0;
			if (passLine.chars[passLinepos] != pass_hyphen) {
				if (!appendInstructionChar(
							file, passInstructions, &passIC, passHoldNumber))
					return 0;
				break;
			}
			passLinepos++;
			passGetNumber(&passLine, &passLinepos, &passHoldNumber);
			if (passHoldNumber == 0) {
				compileError(file, "invalid range");
				return 0;
			}
			if (!appendInstructionChar(file, passInstructions, &passIC, passHoldNumber))
				return 0;
			break;
		case pass_groupstart:
		case pass_groupend: {
			passLinepos++;
			passGetName(&passLine, &passLinepos, &passHoldString);
			TranslationTableOffset ruleOffset = findRuleName(&passHoldString, *table);
			if (ruleOffset)
				rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
			if (rule && rule->opcode == CTO_Grouping) {
				if (!appendInstructionChar(file, passInstructions, &passIC, passSubOp))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, ruleOffset >> 16))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, ruleOffset & 0xffff))
					return 0;
				break;
			} else {
				compileError(file, "%s is not a grouping name",
						_lou_showString(
								&passHoldString.chars[0], passHoldString.length, 0));
				return 0;
			}
			break;
		}
		case pass_swap: {
			passLinepos++;
			passGetName(&passLine, &passLinepos, &passHoldString);
			if ((class = findCharacterClass(&passHoldString, *table))) {
				passAttributes = class->attribute;
				goto insertAttributes;
			}
			TranslationTableOffset ruleOffset = findRuleName(&passHoldString, *table);
			if (ruleOffset)
				rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
			if (rule &&
					(rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd ||
							rule->opcode == CTO_SwapDd)) {
				if (!appendInstructionChar(file, passInstructions, &passIC, pass_swap))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, ruleOffset >> 16))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, ruleOffset & 0xffff))
					return 0;
				goto getRange;
			}
			compileError(file, "%s is neither a class name nor a swap name.",
					_lou_showString(&passHoldString.chars[0], passHoldString.length, 0));
			return 0;
		}
		case pass_endTest:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_endTest))
				return 0;
			passLinepos++;
			break;
		default:
			compileError(file, "incorrect operator '%c ' in test part",
					passLine.chars[passLinepos]);
			return 0;
		}

	} /* Compile action part */

	/* Compile action part */
	while (passLinepos < passLine.length && passLine.chars[passLinepos] <= 32)
		passLinepos++;
	while (passLinepos < passLine.length && passLine.chars[passLinepos] > 32) {
		if (passIC >= MAXSTRING) {
			compileError(file, "Action part in multipass operand too long");
			return 0;
		}
		switch ((passSubOp = passLine.chars[passLinepos])) {
		case pass_string:
			if (!verifyStringOrDots(file, opcode, 1, 1, nofor)) {
				return 0;
			}
			passLinepos++;
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_string))
				return 0;
			passGetString(&passLine, &passLinepos, &passHoldString, file);
			goto actionDoCharsDots;
		case pass_dots:
			if (!verifyStringOrDots(file, opcode, 0, 1, nofor)) {
				return 0;
			}
			passLinepos++;
			passGetDots(&passLine, &passLinepos, &passHoldString, file);
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_dots))
				return 0;
			if (passHoldString.length == 0) {
				compileError(file, "expected dot pattern after @ operand in action part");
				return 0;
			}
		actionDoCharsDots:
			if (passIC >= MAXSTRING) {
				compileError(
						file, "@ operand in action part of multipass operand too long");
				return 0;
			}
			if (!appendInstructionChar(
						file, passInstructions, &passIC, passHoldString.length))
				return 0;
			for (kk = 0; kk < passHoldString.length; kk++) {
				if (passIC >= MAXSTRING) {
					compileError(file,
							"@ operand in action part of multipass operand too long");
					return 0;
				}
				if (!appendInstructionChar(
							file, passInstructions, &passIC, passHoldString.chars[kk]))
					return 0;
			}
			break;
		case pass_variable:
			passLinepos++;
			if (!passGetVariableNumber(file, &passLine, &passLinepos, &passHoldNumber))
				return 0;
			switch (passLine.chars[passLinepos]) {
			case pass_eq:
				if (!appendInstructionChar(file, passInstructions, &passIC, pass_eq))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, passHoldNumber))
					return 0;
				passLinepos++;
				passGetNumber(&passLine, &passLinepos, &passHoldNumber);
				if (!appendInstructionChar(
							file, passInstructions, &passIC, passHoldNumber))
					return 0;
				break;
			case pass_plus:
			case pass_hyphen:
				if (!appendInstructionChar(file, passInstructions, &passIC,
							passLine.chars[passLinepos++]))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, passHoldNumber))
					return 0;
				break;
			default:
				compileError(file, "incorrect variable operator in action part");
				return 0;
			}
			break;
		case pass_copy:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_copy))
				return 0;
			passLinepos++;
			break;
		case pass_omit:
			if (!appendInstructionChar(file, passInstructions, &passIC, pass_omit))
				return 0;
			passLinepos++;
			break;
		case pass_groupreplace:
		case pass_groupstart:
		case pass_groupend: {
			passLinepos++;
			passGetName(&passLine, &passLinepos, &passHoldString);
			TranslationTableOffset ruleOffset = findRuleName(&passHoldString, *table);
			if (ruleOffset)
				rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
			if (rule && rule->opcode == CTO_Grouping) {
				if (!appendInstructionChar(file, passInstructions, &passIC, passSubOp))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, ruleOffset >> 16))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, ruleOffset & 0xffff))
					return 0;
				break;
			}
			compileError(file, "%s is not a grouping name",
					_lou_showString(&passHoldString.chars[0], passHoldString.length, 0));
			return 0;
		}
		case pass_swap: {
			passLinepos++;
			passGetName(&passLine, &passLinepos, &passHoldString);
			TranslationTableOffset ruleOffset = findRuleName(&passHoldString, *table);
			if (ruleOffset)
				rule = (TranslationTableRule *)&(*table)->ruleArea[ruleOffset];
			if (rule &&
					(rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd ||
							rule->opcode == CTO_SwapDd)) {
				if (!appendInstructionChar(file, passInstructions, &passIC, pass_swap))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, ruleOffset >> 16))
					return 0;
				if (!appendInstructionChar(
							file, passInstructions, &passIC, ruleOffset & 0xffff))
					return 0;
				break;
			}
			compileError(file, "%s is not a swap name.",
					_lou_showString(&passHoldString.chars[0], passHoldString.length, 0));
			return 0;
			break;
		}
		default:
			compileError(file, "incorrect operator in action part");
			return 0;
		}
	}

	/* Analyze and add rule */
	passRuleDots.length = passIC;

	{
		widechar *characters;
		int length;
		int found = passFindCharacters(
				file, passInstructions, passRuleDots.length, &characters, &length);

		if (!found) return 0;

		if (characters) {
			for (k = 0; k < length; k += 1) passRuleChars.chars[k] = characters[k];
			passRuleChars.length = k;
		}
	}

	if (!addRule(file, opcode, &passRuleChars, &passRuleDots, 0, 0, NULL, NULL, noback,
				nofor, table))
		return 0;
	return 1;
}