HttpStateData::ReuseDecision::make(const HttpStateData::ReuseDecision::Answers ans, const char *why)
{
    answer = ans;
    reason = why;
    return answer;
}