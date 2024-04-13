int parse_feature_request(const char *feature_list, const char *feature)
{
	return !!parse_feature_value(feature_list, feature, NULL);
}