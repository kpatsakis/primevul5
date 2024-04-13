int server_supports(const char *feature)
{
	return !!server_feature_value(feature, NULL);
}