static inline int dns_resolution_timeout(struct dns_resolution *res)
{
	switch (res->status) {
		case RSLV_STATUS_VALID: return res->resolvers->hold.valid;
		default:                return res->resolvers->timeout.resolve;
	}
}
