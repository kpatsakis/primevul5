static int cli_io_handler_dump_resolvers_to_buffer(struct appctx *appctx)
{
	struct stream_interface *si = appctx->owner;
	struct dns_resolvers    *resolvers;
	struct dns_nameserver   *ns;

	chunk_reset(&trash);

	switch (appctx->st2) {
	case STAT_ST_INIT:
		appctx->st2 = STAT_ST_LIST; /* let's start producing data */
		/* fall through */

	case STAT_ST_LIST:
		if (LIST_ISEMPTY(&dns_resolvers)) {
			chunk_appendf(&trash, "No resolvers found\n");
		}
		else {
			list_for_each_entry(resolvers, &dns_resolvers, list) {
				if (appctx->ctx.cli.p0 != NULL && appctx->ctx.cli.p0 != resolvers)
					continue;

				chunk_appendf(&trash, "Resolvers section %s\n", resolvers->id);
				list_for_each_entry(ns, &resolvers->nameservers, list) {
					chunk_appendf(&trash, " nameserver %s:\n", ns->id);
					chunk_appendf(&trash, "  sent:        %lld\n", ns->counters.sent);
					chunk_appendf(&trash, "  snd_error:   %lld\n", ns->counters.snd_error);
					chunk_appendf(&trash, "  valid:       %lld\n", ns->counters.valid);
					chunk_appendf(&trash, "  update:      %lld\n", ns->counters.update);
					chunk_appendf(&trash, "  cname:       %lld\n", ns->counters.cname);
					chunk_appendf(&trash, "  cname_error: %lld\n", ns->counters.cname_error);
					chunk_appendf(&trash, "  any_err:     %lld\n", ns->counters.any_err);
					chunk_appendf(&trash, "  nx:          %lld\n", ns->counters.nx);
					chunk_appendf(&trash, "  timeout:     %lld\n", ns->counters.timeout);
					chunk_appendf(&trash, "  refused:     %lld\n", ns->counters.refused);
					chunk_appendf(&trash, "  other:       %lld\n", ns->counters.other);
					chunk_appendf(&trash, "  invalid:     %lld\n", ns->counters.invalid);
					chunk_appendf(&trash, "  too_big:     %lld\n", ns->counters.too_big);
					chunk_appendf(&trash, "  truncated:   %lld\n", ns->counters.truncated);
					chunk_appendf(&trash, "  outdated:    %lld\n",  ns->counters.outdated);
				}
				chunk_appendf(&trash, "\n");
			}
		}

		/* display response */
		if (ci_putchk(si_ic(si), &trash) == -1) {
			/* let's try again later from this session. We add ourselves into
			 * this session's users so that it can remove us upon termination.
			 */
			si_rx_room_blk(si);
			return 0;
		}
		/* fall through */

	default:
		appctx->st2 = STAT_ST_FIN;
		return 1;
	}
}
