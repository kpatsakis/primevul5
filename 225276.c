static int oidc_handle_session_management_iframe_rp(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, const char *client_id,
		const char *check_session_iframe) {

	oidc_debug(r, "enter");

	const char *java_script =
			"    <script type=\"text/javascript\">\n"
			"      var targetOrigin  = '%s';\n"
			"      var clientId  = '%s';\n"
			"      var sessionId  = '%s';\n"
			"      var loginUrl  = '%s';\n"
			"      var message = clientId + ' ' + sessionId;\n"
			"	   var timerID;\n"
			"\n"
			"      function checkSession() {\n"
			"        console.debug('checkSession: posting ' + message + ' to ' + targetOrigin);\n"
			"        var win = window.parent.document.getElementById('%s').contentWindow;\n"
			"        win.postMessage( message, targetOrigin);\n"
			"      }\n"
			"\n"
			"      function setTimer() {\n"
			"        checkSession();\n"
			"        timerID = setInterval('checkSession()', %d);\n"
			"      }\n"
			"\n"
			"      function receiveMessage(e) {\n"
			"        console.debug('receiveMessage: ' + e.data + ' from ' + e.origin);\n"
			"        if (e.origin !== targetOrigin ) {\n"
			"          console.debug('receiveMessage: cross-site scripting attack?');\n"
			"          return;\n"
			"        }\n"
			"        if (e.data != 'unchanged') {\n"
			"          clearInterval(timerID);\n"
			"          if (e.data == 'changed' && sessionId == '' ) {\n"
			"			 // 'changed' + no session: enforce a login (if we have a login url...)\n"
			"            if (loginUrl != '') {\n"
			"              window.top.location.replace(loginUrl);\n"
			"            }\n"
			"		   } else {\n"
			"              // either 'changed' + active session, or 'error': enforce a logout\n"
			"              window.top.location.replace('%s?logout=' + encodeURIComponent(window.top.location.href));\n"
			"          }\n"
			"        }\n"
			"      }\n"
			"\n"
			"      window.addEventListener('message', receiveMessage, false);\n"
			"\n"
			"    </script>\n";

	/* determine the origin for the check_session_iframe endpoint */
	char *origin = apr_pstrdup(r->pool, check_session_iframe);
	apr_uri_t uri;
	apr_uri_parse(r->pool, check_session_iframe, &uri);
	char *p = strstr(origin, uri.path);
	*p = '\0';

	/* the element identifier for the OP iframe */
	const char *op_iframe_id = "openidc-op";

	/* restore the OP session_state from the session */
	const char *session_state = oidc_session_get_session_state(r, session);
	if (session_state == NULL) {
		oidc_warn(r,
				"no session_state found in the session; the OP does probably not support session management!?");
		//return OK;
	}

	char *s_poll_interval = NULL;
	oidc_util_get_request_parameter(r, "poll", &s_poll_interval);
	int poll_interval = s_poll_interval ? strtol(s_poll_interval, NULL, 10) : 0;
	if ((poll_interval <= 0) || (poll_interval > 3600 * 24))
		poll_interval = 3000;

	char *login_uri = NULL, *error_str = NULL, *error_description = NULL;
	oidc_util_get_request_parameter(r, "login_uri", &login_uri);
	if ((login_uri != NULL)
			&& (oidc_validate_redirect_url(r, c, login_uri, FALSE, &error_str,
					&error_description) == FALSE)) {
		return HTTP_BAD_REQUEST;
	}

	const char *redirect_uri = oidc_get_redirect_uri(r, c);

	java_script = apr_psprintf(r->pool, java_script, origin, client_id,
			session_state ? session_state : "", login_uri ? login_uri : "",
					op_iframe_id, poll_interval, redirect_uri, redirect_uri);

	return oidc_util_html_send(r, NULL, java_script, "setTimer", NULL, OK);
}