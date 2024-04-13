HttpStateData::checkDateSkew(HttpReply *reply)
{
    if (reply->date > -1 && !_peer) {
        int skew = abs((int)(reply->date - squid_curtime));

        if (skew > 86400)
            debugs(11, 3, "" << request->url.host() << "'s clock is skewed by " << skew << " seconds!");
    }
}