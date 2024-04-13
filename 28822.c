static int cli_parse_stat_resolvers(char **args, char *payload, struct appctx *appctx, void *private)
{
	struct dns_resolvers *presolvers;

	if (*args[2]) {
		list_for_each_entry(presolvers, &dns_resolvers, list) {
			if (strcmp(presolvers->id, args[2]) == 0) {
				appctx->ctx.cli.p0 = presolvers;
				break;
			}
		}
		if (appctx->ctx.cli.p0 == NULL) {
			appctx->ctx.cli.severity = LOG_ERR;
			appctx->ctx.cli.msg = "Can't find that resolvers section\n";
			appctx->st0 = CLI_ST_PRINT;
			return 1;
		}
	}
	return 0;
}
