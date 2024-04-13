HttpStateData::processSurrogateControl(HttpReply *reply)
{
    if (request->flags.accelerated && reply->surrogate_control) {
        HttpHdrScTarget *sctusable = reply->surrogate_control->getMergedTarget(Config.Accel.surrogate_id);

        if (sctusable) {
            if (sctusable->hasNoStore() ||
                    (Config.onoff.surrogate_is_remote
                     && sctusable->noStoreRemote())) {
                surrogateNoStore = true;
                // Be conservative for now and make it non-shareable because
                // there is no enough information here to make the decision.
                entry->makePrivate(false);
            }

            /* The HttpHeader logic cannot tell if the header it's parsing is a reply to an
             * accelerated request or not...
             * Still, this is an abstraction breach. - RC
             */
            if (sctusable->hasMaxAge()) {
                if (sctusable->maxAge() < sctusable->maxStale())
                    reply->expires = reply->date + sctusable->maxAge();
                else
                    reply->expires = reply->date + sctusable->maxStale();

                /* And update the timestamps */
                entry->timestampsSet();
            }

            /* We ignore cache-control directives as per the Surrogate specification */
            ignoreCacheControl = true;

            delete sctusable;
        }
    }
}