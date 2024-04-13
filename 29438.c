static int labelof(const struct in6_addr *a)
{
	return policyof(a)->label;
}
