static int phar_copy_file_contents(phar_entry_info *entry, php_stream *fp TSRMLS_DC) /* {{{ */
{
	char *error;
	off_t offset;
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(entry, &error, 1 TSRMLS_CC)) {
		if (error) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot convert phar archive \"%s\", unable to open entry \"%s\" contents: %s", entry->phar->fname, entry->filename, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot convert phar archive \"%s\", unable to open entry \"%s\" contents", entry->phar->fname, entry->filename);
		}
		return FAILURE;
	}

	/* copy old contents in entirety */
	phar_seek_efp(entry, 0, SEEK_SET, 0, 1 TSRMLS_CC);
	offset = php_stream_tell(fp);
	link = phar_get_link_source(entry TSRMLS_CC);

	if (!link) {
		link = entry;
	}

	if (SUCCESS != phar_stream_copy_to_stream(phar_get_efp(link, 0 TSRMLS_CC), fp, link->uncompressed_filesize, NULL)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot convert phar archive \"%s\", unable to copy entry \"%s\" contents", entry->phar->fname, entry->filename);
		return FAILURE;
	}

	if (entry->fp_type == PHAR_MOD) {
		/* save for potential restore on error */
		entry->cfp = entry->fp;
		entry->fp = NULL;
	}

	/* set new location of file contents */
	entry->fp_type = PHAR_FP;
	entry->offset = offset;
	return SUCCESS;
}