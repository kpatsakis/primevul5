Value ExpressionDateFromParts::evaluate(const Document& root, Variables* variables) const {
    long long hour, minute, second, millisecond;

    if (!evaluateNumberWithDefaultAndBounds(root, _hour.get(), "hour"_sd, 0, &hour, variables) ||
        !evaluateNumberWithDefaultAndBounds(
            root, _minute.get(), "minute"_sd, 0, &minute, variables) ||
        !evaluateNumberWithDefault(root, _second.get(), "second"_sd, 0, &second, variables) ||
        !evaluateNumberWithDefault(
            root, _millisecond.get(), "millisecond"_sd, 0, &millisecond, variables)) {
        // One of the evaluated inputs in nullish.
        return Value(BSONNULL);
    }

    auto timeZone =
        makeTimeZone(getExpressionContext()->timeZoneDatabase, root, _timeZone.get(), variables);

    if (!timeZone) {
        return Value(BSONNULL);
    }

    if (_year) {
        long long year, month, day;

        if (!evaluateNumberWithDefault(root, _year.get(), "year"_sd, 1970, &year, variables) ||
            !evaluateNumberWithDefaultAndBounds(
                root, _month.get(), "month"_sd, 1, &month, variables) ||
            !evaluateNumberWithDefaultAndBounds(root, _day.get(), "day"_sd, 1, &day, variables)) {
            // One of the evaluated inputs in nullish.
            return Value(BSONNULL);
        }

        uassert(40523,
                str::stream() << "'year' must evaluate to an integer in the range " << 1 << " to "
                              << 9999 << ", found " << year,
                year >= 1 && year <= 9999);

        return Value(
            timeZone->createFromDateParts(year, month, day, hour, minute, second, millisecond));
    }

    if (_isoWeekYear) {
        long long isoWeekYear, isoWeek, isoDayOfWeek;

        if (!evaluateNumberWithDefault(
                root, _isoWeekYear.get(), "isoWeekYear"_sd, 1970, &isoWeekYear, variables) ||
            !evaluateNumberWithDefaultAndBounds(
                root, _isoWeek.get(), "isoWeek"_sd, 1, &isoWeek, variables) ||
            !evaluateNumberWithDefaultAndBounds(
                root, _isoDayOfWeek.get(), "isoDayOfWeek"_sd, 1, &isoDayOfWeek, variables)) {
            // One of the evaluated inputs in nullish.
            return Value(BSONNULL);
        }

        uassert(31095,
                str::stream() << "'isoWeekYear' must evaluate to an integer in the range " << 1
                              << " to " << 9999 << ", found " << isoWeekYear,
                isoWeekYear >= 1 && isoWeekYear <= 9999);

        return Value(timeZone->createFromIso8601DateParts(
            isoWeekYear, isoWeek, isoDayOfWeek, hour, minute, second, millisecond));
    }

    MONGO_UNREACHABLE;
}