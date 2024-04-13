HttpStateData::processReply()
{

    if (flags.handling1xx) { // we came back after handling a 1xx response
        debugs(11, 5, HERE << "done with 1xx handling");
        flags.handling1xx = false;
        Must(!flags.headers_parsed);
    }

    if (!flags.headers_parsed) { // have not parsed headers yet?
        PROF_start(HttpStateData_processReplyHeader);
        processReplyHeader();
        PROF_stop(HttpStateData_processReplyHeader);

        if (!continueAfterParsingHeader()) // parsing error or need more data
            return; // TODO: send errors to ICAP

        adaptOrFinalizeReply(); // may write to, abort, or "close" the entry
    }

    // kick more reads if needed and/or process the response body, if any
    PROF_start(HttpStateData_processReplyBody);
    processReplyBody(); // may call serverComplete()
    PROF_stop(HttpStateData_processReplyBody);
}