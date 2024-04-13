static void dns_reset_resolution(struct dns_resolution *resolution)
{
	/* update resolution status */
	resolution->step            = RSLV_STEP_NONE;
	resolution->try             = 0;
	resolution->last_resolution = now_ms;
	resolution->nb_queries      = 0;
	resolution->nb_responses    = 0;
	resolution->query_type      = resolution->prefered_query_type;

	/* clean up query id */
	eb32_delete(&resolution->qid);
	resolution->query_id = 0;
	resolution->qid.key   = 0;
}
