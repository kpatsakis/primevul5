struct ref **get_remote_heads(int in, char *src_buf, size_t src_len,
			      struct ref **list, unsigned int flags,
			      struct oid_array *extra_have,
			      struct oid_array *shallow_points)
{
	struct ref **orig_list = list;

	/*
	 * A hang-up after seeing some response from the other end
	 * means that it is unexpected, as we know the other end is
	 * willing to talk to us.  A hang-up before seeing any
	 * response does not necessarily mean an ACL problem, though.
	 */
	int saw_response;
	int got_dummy_ref_with_capabilities_declaration = 0;

	*list = NULL;
	for (saw_response = 0; ; saw_response = 1) {
		struct ref *ref;
		struct object_id old_oid;
		char *name;
		int len, name_len;
		char *buffer = packet_buffer;
		const char *arg;

		len = packet_read(in, &src_buf, &src_len,
				  packet_buffer, sizeof(packet_buffer),
				  PACKET_READ_GENTLE_ON_EOF |
				  PACKET_READ_CHOMP_NEWLINE);
		if (len < 0)
			die_initial_contact(saw_response);

		if (!len)
			break;

		if (len > 4 && skip_prefix(buffer, "ERR ", &arg))
			die("remote error: %s", arg);

		if (len == GIT_SHA1_HEXSZ + strlen("shallow ") &&
			skip_prefix(buffer, "shallow ", &arg)) {
			if (get_oid_hex(arg, &old_oid))
				die("protocol error: expected shallow sha-1, got '%s'", arg);
			if (!shallow_points)
				die("repository on the other end cannot be shallow");
			oid_array_append(shallow_points, &old_oid);
			continue;
		}

		if (len < GIT_SHA1_HEXSZ + 2 || get_oid_hex(buffer, &old_oid) ||
			buffer[GIT_SHA1_HEXSZ] != ' ')
			die("protocol error: expected sha/ref, got '%s'", buffer);
		name = buffer + GIT_SHA1_HEXSZ + 1;

		name_len = strlen(name);
		if (len != name_len + GIT_SHA1_HEXSZ + 1) {
			free(server_capabilities);
			server_capabilities = xstrdup(name + name_len + 1);
		}

		if (extra_have && !strcmp(name, ".have")) {
			oid_array_append(extra_have, &old_oid);
			continue;
		}

		if (!strcmp(name, "capabilities^{}")) {
			if (saw_response)
				die("protocol error: unexpected capabilities^{}");
			if (got_dummy_ref_with_capabilities_declaration)
				die("protocol error: multiple capabilities^{}");
			got_dummy_ref_with_capabilities_declaration = 1;
			continue;
		}

		if (!check_ref(name, flags))
			continue;

		if (got_dummy_ref_with_capabilities_declaration)
			die("protocol error: unexpected ref after capabilities^{}");

		ref = alloc_ref(buffer + GIT_SHA1_HEXSZ + 1);
		oidcpy(&ref->old_oid, &old_oid);
		*list = ref;
		list = &ref->next;
	}

	annotate_refs_with_symref_info(*orig_list);

	return list;
}