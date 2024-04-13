passFindCharacters(const FileInfo *file, widechar *instructions, int end,
		widechar **characters, int *length) {
	int IC = 0;
	int lookback = 0;

	*characters = NULL;
	*length = 0;

	while (IC < end) {
		widechar instruction = instructions[IC];

		switch (instruction) {
		case pass_string:
		case pass_dots: {
			int count = instructions[IC + 1];
			IC += 2;
			if (count > lookback) {
				*characters = &instructions[IC + lookback];
				*length = count - lookback;
				return 1;
			} else {
				lookback -= count;
			}
			IC += count;
			continue;
		}

		case pass_attributes:
			IC += 7;
			if (instructions[IC - 2] == instructions[IC - 1] &&
					instructions[IC - 1] <= lookback) {
				lookback -= instructions[IC - 1];
				continue;
			}
			goto NO_CHARACTERS;

		case pass_swap:
			IC += 2;
			/* fall through */

		case pass_groupstart:
		case pass_groupend:
		case pass_groupreplace:
			IC += 3;

		NO_CHARACTERS : { return 1; }

		case pass_eq:
		case pass_lt:
		case pass_gt:
		case pass_lteq:
		case pass_gteq:
			IC += 3;
			continue;

		case pass_lookback:
			lookback += instructions[IC + 1];
			IC += 2;
			continue;

		case pass_not:
		case pass_startReplace:
		case pass_endReplace:
		case pass_first:
		case pass_last:
		case pass_copy:
		case pass_omit:
		case pass_plus:
		case pass_hyphen:
			IC += 1;
			continue;

		case pass_endTest:
			goto NO_CHARACTERS;

		default:
			compileError(file, "unhandled test suboperand: \\x%02x", instruction);
			return 0;
		}
	}
	goto NO_CHARACTERS;
}