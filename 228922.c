filter_protocol_internal(struct filter_session *fs, uint64_t *token, uint64_t reqid, enum filter_phase phase, const char *param)
{
	struct filter_chain	*filter_chain;
	struct filter_entry	*filter_entry;
	struct filter		*filter;
	struct timeval		 tv;
	const char		*phase_name = filter_execs[phase].phase_name;
	int			 resume = 1;

	if (!*token) {
		fs->phase = phase;
		resume = 0;
	}

	/* XXX - this sanity check requires a protocol change, stub for now */
	phase = fs->phase;
	if (fs->phase != phase)
		fatalx("misbehaving filter");

	/* based on token, identify the filter_entry we should apply  */
	filter_chain = dict_get(&filter_chains, fs->filter_name);
	filter_entry = TAILQ_FIRST(&filter_chain->chain[fs->phase]);
	if (*token) {
		TAILQ_FOREACH(filter_entry, &filter_chain->chain[fs->phase], entries)
		    if (filter_entry->id == *token)
			    break;
		if (filter_entry == NULL)
			fatalx("misbehaving filter");
		filter_entry = TAILQ_NEXT(filter_entry, entries);
	}

	/* no filter_entry, we either had none or reached end of chain */
	if (filter_entry == NULL) {
		log_trace(TRACE_FILTERS, "%016"PRIx64" filters protocol phase=%s, resume=%s, "
		    "action=proceed",
		    fs->id, phase_name, resume ? "y" : "n");
		filter_result_proceed(reqid);
		return;
	}

	/* process param with current filter_entry */
	*token = filter_entry->id;
	filter = dict_get(&filters, filter_entry->name);
	if (filter->proc) {
		log_trace(TRACE_FILTERS, "%016"PRIx64" filters protocol phase=%s, "
		    "resume=%s, action=deferred, filter=%s",
		    fs->id, phase_name, resume ? "y" : "n",
		    filter->name);
		filter_protocol_query(filter, filter_entry->id, reqid,
		    filter_execs[fs->phase].phase_name, param);
		return;	/* deferred response */
	}

	if (filter_execs[fs->phase].func(fs, filter, reqid, param)) {
		if (filter->config->rewrite) {
			log_trace(TRACE_FILTERS, "%016"PRIx64" filters protocol phase=%s, "
			    "resume=%s, action=rewrite, filter=%s, query=%s, response=%s",
			    fs->id, phase_name, resume ? "y" : "n",
			    filter->name,
			    param,
			    filter->config->rewrite);
			    filter_result_rewrite(reqid, filter->config->rewrite);
			return;
		}
		else if (filter->config->disconnect) {
			log_trace(TRACE_FILTERS, "%016"PRIx64" filters protocol phase=%s, "
			    "resume=%s, action=disconnect, filter=%s, query=%s, response=%s",
			    fs->id, phase_name, resume ? "y" : "n",
			    filter->name,
			    param,
			    filter->config->disconnect);
			filter_result_disconnect(reqid, filter->config->disconnect);
			return;
		}
		else if (filter->config->junk) {
			log_trace(TRACE_FILTERS, "%016"PRIx64" filters protocol phase=%s, "
			    "resume=%s, action=junk, filter=%s, query=%s",
			    fs->id, phase_name, resume ? "y" : "n",
			    filter->name,
			    param);
			filter_result_junk(reqid);
			return;
		} else if (filter->config->report) {
			log_trace(TRACE_FILTERS, "%016"PRIx64" filters protocol phase=%s, "
			    "resume=%s, action=report, filter=%s, query=%s response=%s",
			    fs->id, phase_name, resume ? "y" : "n",
			    filter->name,
			    param, filter->config->report);

			gettimeofday(&tv, NULL);
			lka_report_filter_report(fs->id, filter->name, 1,
			    "smtp-in", &tv, filter->config->report);
		} else if (filter->config->bypass) {
			log_trace(TRACE_FILTERS, "%016"PRIx64" filters protocol phase=%s, "
			    "resume=%s, action=bypass, filter=%s, query=%s",
			    fs->id, phase_name, resume ? "y" : "n",
			    filter->name,
			    param);
			filter_result_proceed(reqid);
			return;
		} else {
			log_trace(TRACE_FILTERS, "%016"PRIx64" filters protocol phase=%s, "
			    "resume=%s, action=reject, filter=%s, query=%s, response=%s",
			    fs->id, phase_name, resume ? "y" : "n",
			    filter->name,
			    param,
			    filter->config->reject);
			filter_result_reject(reqid, filter->config->reject);
			return;
		}
	}

	log_trace(TRACE_FILTERS, "%016"PRIx64" filters protocol phase=%s, "
	    "resume=%s, action=proceed, filter=%s, query=%s",
	    fs->id, phase_name, resume ? "y" : "n",
	    filter->name,
	    param);

	/* filter_entry resulted in proceed, try next filter */
	filter_protocol_internal(fs, token, reqid, phase, param);
	return;
}