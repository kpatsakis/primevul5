static bool split_dfs_path(TALLOC_CTX *ctx,
				const char *nodepath,
				char **pp_server,
				char **pp_share,
				char **pp_extrapath)
{
	char *p, *q;
	char *path;

	*pp_server = NULL;
	*pp_share = NULL;
	*pp_extrapath = NULL;

	path = talloc_strdup(ctx, nodepath);
	if (!path) {
		goto fail;
	}

	if ( path[0] != '\\' ) {
		goto fail;
	}

	p = strchr_m( path + 1, '\\' );
	if ( !p ) {
		goto fail;
	}

	*p = '\0';
	p++;

	/* Look for any extra/deep path */
	q = strchr_m(p, '\\');
	if (q != NULL) {
		*q = '\0';
		q++;
		*pp_extrapath = talloc_strdup(ctx, q);
	} else {
		*pp_extrapath = talloc_strdup(ctx, "");
	}
	if (*pp_extrapath == NULL) {
		goto fail;
	}

	*pp_share = talloc_strdup(ctx, p);
	if (*pp_share == NULL) {
		goto fail;
	}

	*pp_server = talloc_strdup(ctx, &path[1]);
	if (*pp_server == NULL) {
		goto fail;
	}

	TALLOC_FREE(path);
	return true;

fail:
	TALLOC_FREE(*pp_share);
	TALLOC_FREE(*pp_extrapath);
	TALLOC_FREE(path);
	return false;
}
