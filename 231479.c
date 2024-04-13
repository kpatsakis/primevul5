int trace_get_user(struct trace_parser *parser, const char __user *ubuf,
	size_t cnt, loff_t *ppos)
{
	char ch;
	size_t read = 0;
	ssize_t ret;

	if (!*ppos)
		trace_parser_clear(parser);

	ret = get_user(ch, ubuf++);
	if (ret)
		goto out;

	read++;
	cnt--;

	/*
	 * The parser is not finished with the last write,
	 * continue reading the user input without skipping spaces.
	 */
	if (!parser->cont) {
		/* skip white space */
		while (cnt && isspace(ch)) {
			ret = get_user(ch, ubuf++);
			if (ret)
				goto out;
			read++;
			cnt--;
		}

		/* only spaces were written */
		if (isspace(ch)) {
			*ppos += read;
			ret = read;
			goto out;
		}

		parser->idx = 0;
	}

	/* read the non-space input */
	while (cnt && !isspace(ch)) {
		if (parser->idx < parser->size - 1)
			parser->buffer[parser->idx++] = ch;
		else {
			ret = -EINVAL;
			goto out;
		}
		ret = get_user(ch, ubuf++);
		if (ret)
			goto out;
		read++;
		cnt--;
	}

	/* We either got finished input or we have to wait for another call. */
	if (isspace(ch)) {
		parser->buffer[parser->idx] = 0;
		parser->cont = false;
	} else if (parser->idx < parser->size - 1) {
		parser->cont = true;
		parser->buffer[parser->idx++] = ch;
	} else {
		ret = -EINVAL;
		goto out;
	}

	*ppos += read;
	ret = read;

out:
	return ret;
}