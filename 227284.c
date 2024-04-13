HttpStateData::processReplyBody()
{
    if (!flags.headers_parsed) {
        flags.do_next_read = true;
        maybeReadVirginBody();
        return;
    }

#if USE_ADAPTATION
    debugs(11,5, HERE << "adaptationAccessCheckPending=" << adaptationAccessCheckPending);
    if (adaptationAccessCheckPending)
        return;

#endif

    /*
     * At this point the reply headers have been parsed and consumed.
     * That means header content has been removed from readBuf and
     * it contains only body data.
     */
    if (entry->isAccepting()) {
        if (flags.chunked) {
            if (!decodeAndWriteReplyBody()) {
                flags.do_next_read = false;
                serverComplete();
                return;
            }
        } else
            writeReplyBody();
    }

    // storing/sending methods like earlier adaptOrFinalizeReply() or
    // above writeReplyBody() may release/abort the store entry.
    if (EBIT_TEST(entry->flags, ENTRY_ABORTED)) {
        // TODO: In some cases (e.g., 304), we should keep persistent conn open.
        // Detect end-of-reply (and, hence, pool our idle pconn) earlier (ASAP).
        abortTransaction("store entry aborted while storing reply");
        return;
    } else
        switch (persistentConnStatus()) {
        case INCOMPLETE_MSG: {
            debugs(11, 5, "processReplyBody: INCOMPLETE_MSG from " << serverConnection);
            /* Wait for more data or EOF condition */
            AsyncCall::Pointer nil;
            if (flags.keepalive_broken) {
                commSetConnTimeout(serverConnection, 10, nil);
            } else {
                commSetConnTimeout(serverConnection, Config.Timeout.read, nil);
            }

            flags.do_next_read = true;
        }
        break;

        case COMPLETE_PERSISTENT_MSG: {
            debugs(11, 5, "processReplyBody: COMPLETE_PERSISTENT_MSG from " << serverConnection);

            // TODO: Remove serverConnectionSaved but preserve exception safety.

            commUnsetConnTimeout(serverConnection);
            flags.do_next_read = false;

            comm_remove_close_handler(serverConnection->fd, closeHandler);
            closeHandler = NULL;

            Ip::Address client_addr; // XXX: Remove as unused. Why was it added?
            if (request->flags.spoofClientIp)
                client_addr = request->client_addr;

            auto serverConnectionSaved = serverConnection;
            fwd->unregister(serverConnection);
            serverConnection = nullptr;

            bool ispinned = false; // TODO: Rename to isOrShouldBePinned
            if (request->flags.pinned) {
                ispinned = true;
            } else if (request->flags.connectionAuth && request->flags.authSent) {
                ispinned = true;
            }

            if (ispinned) {
                if (request->clientConnectionManager.valid()) {
                    CallJobHere1(11, 4, request->clientConnectionManager,
                                 ConnStateData,
                                 notePinnedConnectionBecameIdle,
                                 ConnStateData::PinnedIdleContext(serverConnectionSaved, request));
                } else {
                    // must not pool/share ispinned connections, even orphaned ones
                    serverConnectionSaved->close();
                }
            } else {
                fwd->pconnPush(serverConnectionSaved, request->url.host());
            }

            serverComplete();
            return;
        }

        case COMPLETE_NONPERSISTENT_MSG:
            debugs(11, 5, "processReplyBody: COMPLETE_NONPERSISTENT_MSG from " << serverConnection);
            if (flags.chunked && !lastChunk)
                entry->lengthWentBad("missing last-chunk");

            serverComplete();
            return;
        }

    maybeReadVirginBody();
}