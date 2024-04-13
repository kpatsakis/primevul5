getAChar(FileInfo *file) {
	/* Read a big endian, little endian or ASCII 8 file and convert it to
	 * 16- or 32-bit unsigned integers */
	int ch1 = 0, ch2 = 0;
	widechar character;
	if (file->encoding == ascii8)
		if (file->status == 2) {
			file->status++;
			return file->checkencoding[1];
		}
	while ((ch1 = fgetc(file->in)) != EOF) {
		if (file->status < 2) file->checkencoding[file->status] = ch1;
		file->status++;
		if (file->status == 2) {
			if (file->checkencoding[0] == 0xfe && file->checkencoding[1] == 0xff)
				file->encoding = bigEndian;
			else if (file->checkencoding[0] == 0xff && file->checkencoding[1] == 0xfe)
				file->encoding = littleEndian;
			else if (file->checkencoding[0] < 128 && file->checkencoding[1] < 128) {
				file->encoding = ascii8;
				return file->checkencoding[0];
			} else {
				compileError(file,
						"encoding is neither big-endian, little-endian nor ASCII 8.");
				ch1 = EOF;
				break;
				;
			}
			continue;
		}
		switch (file->encoding) {
		case noEncoding:
			break;
		case ascii8:
			return ch1;
			break;
		case bigEndian:
			ch2 = fgetc(file->in);
			if (ch2 == EOF) break;
			character = (widechar)(ch1 << 8) | ch2;
			return (int)character;
			break;
		case littleEndian:
			ch2 = fgetc(file->in);
			if (ch2 == EOF) break;
			character = (widechar)(ch2 << 8) | ch1;
			return (int)character;
			break;
		}
		if (ch1 == EOF || ch2 == EOF) break;
	}
	return EOF;
}