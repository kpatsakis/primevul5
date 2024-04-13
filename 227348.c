HttpStateData::decideIfWeDoRanges (HttpRequest * request)
{
    bool result = true;
    /* decide if we want to do Ranges ourselves
     * and fetch the whole object now)
     * We want to handle Ranges ourselves iff
     *    - we can actually parse client Range specs
     *    - the specs are expected to be simple enough (e.g. no out-of-order ranges)
     *    - reply will be cachable
     * (If the reply will be uncachable we have to throw it away after
     *  serving this request, so it is better to forward ranges to
     *  the server and fetch only the requested content)
     */

    int64_t roffLimit = request->getRangeOffsetLimit();

    if (NULL == request->range || !request->flags.cachable
            || request->range->offsetLimitExceeded(roffLimit) || request->flags.connectionAuth)
        result = false;

    debugs(11, 8, "decideIfWeDoRanges: range specs: " <<
           request->range << ", cachable: " <<
           request->flags.cachable << "; we_do_ranges: " << result);

    return result;
}