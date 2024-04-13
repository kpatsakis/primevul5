_lou_allocMem(AllocBuf buffer, int index, int srcmax, int destmax) {
	if (srcmax < 1024) srcmax = 1024;
	if (destmax < 1024) destmax = 1024;
	switch (buffer) {
	case alloc_typebuf:
		if (destmax > sizeTypebuf) {
			if (typebuf != NULL) free(typebuf);
			// TODO: should this be srcmax?
			typebuf = malloc((destmax + 4) * sizeof(formtype));
			if (!typebuf) _lou_outOfMemory();
			sizeTypebuf = destmax;
		}
		return typebuf;

	case alloc_wordBuffer:

		if (wordBuffer != NULL) free(wordBuffer);
		wordBuffer = calloc(srcmax + 4, sizeof(unsigned int));
		if (wordBuffer == NULL) _lou_outOfMemory();
		return wordBuffer;

	case alloc_emphasisBuffer:

		if (emphasisBuffer != NULL) free(emphasisBuffer);
		emphasisBuffer = calloc(srcmax + 4, sizeof(EmphasisInfo));
		if (emphasisBuffer == NULL) _lou_outOfMemory();
		return emphasisBuffer;

	case alloc_destSpacing:
		if (destmax > sizeDestSpacing) {
			if (destSpacing != NULL) free(destSpacing);
			destSpacing = malloc(destmax + 4);
			if (!destSpacing) _lou_outOfMemory();
			sizeDestSpacing = destmax;
		}
		return destSpacing;
	case alloc_passbuf:
		if (index < 0 || index >= MAXPASSBUF) {
			_lou_logMessage(LOU_LOG_FATAL, "Index out of bounds: %d\n", index);
			exit(3);
		}
		if (destmax > sizePassbuf[index]) {
			if (passbuf[index] != NULL) free(passbuf[index]);
			passbuf[index] = malloc((destmax + 4) * CHARSIZE);
			if (!passbuf[index]) _lou_outOfMemory();
			sizePassbuf[index] = destmax;
		}
		return passbuf[index];
	case alloc_posMapping1: {
		int mapSize;
		if (srcmax >= destmax)
			mapSize = srcmax;
		else
			mapSize = destmax;
		if (mapSize > sizePosMapping1) {
			if (posMapping1 != NULL) free(posMapping1);
			posMapping1 = malloc((mapSize + 4) * sizeof(int));
			if (!posMapping1) _lou_outOfMemory();
			sizePosMapping1 = mapSize;
		}
	}
		return posMapping1;
	case alloc_posMapping2: {
		int mapSize;
		if (srcmax >= destmax)
			mapSize = srcmax;
		else
			mapSize = destmax;
		if (mapSize > sizePosMapping2) {
			if (posMapping2 != NULL) free(posMapping2);
			posMapping2 = malloc((mapSize + 4) * sizeof(int));
			if (!posMapping2) _lou_outOfMemory();
			sizePosMapping2 = mapSize;
		}
	}
		return posMapping2;
	case alloc_posMapping3: {
		int mapSize;
		if (srcmax >= destmax)
			mapSize = srcmax;
		else
			mapSize = destmax;
		if (mapSize > sizePosMapping3) {
			if (posMapping3 != NULL) free(posMapping3);
			posMapping3 = malloc((mapSize + 4) * sizeof(int));
			if (!posMapping3) _lou_outOfMemory();
			sizePosMapping3 = mapSize;
		}
	}
		return posMapping3;
	default:
		return NULL;
	}
}