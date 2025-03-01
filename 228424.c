static zval *phar_rename_archive(phar_archive_data *phar, char *ext, zend_bool compress TSRMLS_DC) /* {{{ */
{
	const char *oldname = NULL;
	char *oldpath = NULL;
	char *basename = NULL, *basepath = NULL;
	char *newname = NULL, *newpath = NULL;
	zval *ret, arg1;
	zend_class_entry *ce;
	char *error;
	const char *pcr_error;
	int ext_len = ext ? strlen(ext) : 0;
	int oldname_len;
	phar_archive_data **pphar = NULL;
	php_stream_statbuf ssb;

	if (!ext) {
		if (phar->is_zip) {

			if (phar->is_data) {
				ext = "zip";
			} else {
				ext = "phar.zip";
			}

		} else if (phar->is_tar) {

			switch (phar->flags) {
				case PHAR_FILE_COMPRESSED_GZ:
					if (phar->is_data) {
						ext = "tar.gz";
					} else {
						ext = "phar.tar.gz";
					}
					break;
				case PHAR_FILE_COMPRESSED_BZ2:
					if (phar->is_data) {
						ext = "tar.bz2";
					} else {
						ext = "phar.tar.bz2";
					}
					break;
				default:
					if (phar->is_data) {
						ext = "tar";
					} else {
						ext = "phar.tar";
					}
			}
		} else {

			switch (phar->flags) {
				case PHAR_FILE_COMPRESSED_GZ:
					ext = "phar.gz";
					break;
				case PHAR_FILE_COMPRESSED_BZ2:
					ext = "phar.bz2";
					break;
				default:
					ext = "phar";
			}
		}
	} else if (phar_path_check(&ext, &ext_len, &pcr_error) > pcr_is_ok) {

		if (phar->is_data) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "data phar converted from \"%s\" has invalid extension %s", phar->fname, ext);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "phar converted from \"%s\" has invalid extension %s", phar->fname, ext);
		}
		return NULL;
	}

	if (ext[0] == '.') {
		++ext;
	}

	oldpath = estrndup(phar->fname, phar->fname_len);
	oldname = zend_memrchr(phar->fname, '/', phar->fname_len);
	++oldname;
	oldname_len = strlen(oldname);

	basename = estrndup(oldname, oldname_len);
	spprintf(&newname, 0, "%s.%s", strtok(basename, "."), ext);
	efree(basename);

	

	basepath = estrndup(oldpath, (strlen(oldpath) - oldname_len));
	phar->fname_len = spprintf(&newpath, 0, "%s%s", basepath, newname);
	phar->fname = newpath;
	phar->ext = newpath + phar->fname_len - strlen(ext) - 1;
	efree(basepath);
	efree(newname);

	if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_find(&cached_phars, newpath, phar->fname_len, (void **) &pphar)) {
		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to add newly converted phar \"%s\" to the list of phars, new phar name is in phar.cache_list", phar->fname);
		return NULL;
	}

	if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), newpath, phar->fname_len, (void **) &pphar)) {
		if ((*pphar)->fname_len == phar->fname_len && !memcmp((*pphar)->fname, phar->fname, phar->fname_len)) {
			if (!zend_hash_num_elements(&phar->manifest)) {
				(*pphar)->is_tar = phar->is_tar;
				(*pphar)->is_zip = phar->is_zip;
				(*pphar)->is_data = phar->is_data;
				(*pphar)->flags = phar->flags;
				(*pphar)->fp = phar->fp;
				phar->fp = NULL;
				phar_destroy_phar_data(phar TSRMLS_CC);
				phar = *pphar;
				phar->refcount++;
				newpath = oldpath;
				goto its_ok;
			}
		}

		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to add newly converted phar \"%s\" to the list of phars, a phar with that name already exists", phar->fname);
		return NULL;
	}
its_ok:
	if (SUCCESS == php_stream_stat_path(newpath, &ssb)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "phar \"%s\" exists and must be unlinked prior to conversion", newpath);
		efree(oldpath);
		return NULL;
	}
	if (!phar->is_data) {
		if (SUCCESS != phar_detect_phar_fname_ext(newpath, phar->fname_len, (const char **) &(phar->ext), &(phar->ext_len), 1, 1, 1 TSRMLS_CC)) {
			efree(oldpath);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "phar \"%s\" has invalid extension %s", phar->fname, ext);
			return NULL;
		}

		if (phar->alias) {
			if (phar->is_temporary_alias) {
				phar->alias = NULL;
				phar->alias_len = 0;
			} else {
				phar->alias = estrndup(newpath, strlen(newpath));
				phar->alias_len = strlen(newpath);
				phar->is_temporary_alias = 1;
				zend_hash_update(&(PHAR_GLOBALS->phar_alias_map), newpath, phar->fname_len, (void*)&phar, sizeof(phar_archive_data*), NULL);
			}
		}

	} else {

		if (SUCCESS != phar_detect_phar_fname_ext(newpath, phar->fname_len, (const char **) &(phar->ext), &(phar->ext_len), 0, 1, 1 TSRMLS_CC)) {
			efree(oldpath);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "data phar \"%s\" has invalid extension %s", phar->fname, ext);
			return NULL;
		}

		phar->alias = NULL;
		phar->alias_len = 0;
	}

	if ((!pphar || phar == *pphar) && SUCCESS != zend_hash_update(&(PHAR_GLOBALS->phar_fname_map), newpath, phar->fname_len, (void*)&phar, sizeof(phar_archive_data*), NULL)) {
		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to add newly converted phar \"%s\" to the list of phars", phar->fname);
		return NULL;
	}

	phar_flush(phar, 0, 0, 1, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%s", error);
		efree(error);
		efree(oldpath);
		return NULL;
	}

	efree(oldpath);

	if (phar->is_data) {
		ce = phar_ce_data;
	} else {
		ce = phar_ce_archive;
	}

	MAKE_STD_ZVAL(ret);

	if (SUCCESS != object_init_ex(ret, ce)) {
		zval_dtor(ret);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to instantiate phar object when converting archive \"%s\"", phar->fname);
		return NULL;
	}

	INIT_PZVAL(&arg1);
	ZVAL_STRINGL(&arg1, phar->fname, phar->fname_len, 0);

	zend_call_method_with_1_params(&ret, ce, &ce->constructor, "__construct", NULL, &arg1);
	return ret;
}