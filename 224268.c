int check_ref_type(const struct ref *ref, int flags)
{
	return check_ref(ref->name, flags);
}