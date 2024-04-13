_PUBLIC_ char *talloc_strdup_upper(TALLOC_CTX *ctx, const char *src)
{
	return strupper_talloc(ctx, src);
}
