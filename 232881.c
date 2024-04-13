table_dump(struct table *t)
{
	const char *type;
	char buf[LINE_MAX];

	switch(t->t_type) {
	case T_NONE:
		type = "NONE";
		break;
	case T_DYNAMIC:
		type = "DYNAMIC";
		break;
	case T_LIST:
		type = "LIST";
		break;
	case T_HASH:
		type = "HASH";
		break;
	default:
		type = "???";
		break;
	}

	if (t->t_config[0])
		snprintf(buf, sizeof(buf), " config=\"%s\"", t->t_config);
	else
		buf[0] = '\0';

	log_debug("TABLE \"%s\" backend=%s type=%s%s", t->t_name,
	    t->t_backend->name, type, buf);

	if (t->t_backend->dump)
		t->t_backend->dump(t);
}