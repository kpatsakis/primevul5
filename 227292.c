ConnStateData::connFinishedWithConn(int size)
{
    if (size == 0) {
        if (pipeline.empty() && inBuf.isEmpty()) {
            /* no current or pending requests */
            debugs(33, 4, HERE << clientConnection << " closed");
            return true;
        } else if (!Config.onoff.half_closed_clients) {
            /* admin doesn't want to support half-closed client sockets */
            debugs(33, 3, HERE << clientConnection << " aborted (half_closed_clients disabled)");
            pipeline.terminateAll(0);
            return true;
        }
    }

    return false;
}