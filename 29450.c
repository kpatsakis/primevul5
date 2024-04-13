copy_resource(fz_context *ctx, pdf_filter_processor *p, pdf_obj *key, const char *name)
{
	pdf_obj *res, *obj;

	if (!name || name[0] == 0)
		return;

	res = pdf_dict_get(ctx, p->old_rdb, key);
	obj = pdf_dict_gets(ctx, res, name);
	if (obj)
	{
		res = pdf_dict_get(ctx, p->new_rdb, key);
		if (!res)
		{
			res = pdf_new_dict(ctx, pdf_get_bound_document(ctx, p->new_rdb), 1);
			pdf_dict_put_drop(ctx, p->new_rdb, key, res);
		}
		pdf_dict_putp(ctx, res, name, obj);
	}
}
