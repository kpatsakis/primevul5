    static Time parseFileTime (uint32 time, uint32 date) noexcept
    {
        auto year      = (int) (1980 + (date >> 9));
        auto month     = (int) (((date >> 5) & 15) - 1);
        auto day       = (int) (date & 31);
        auto hours     = (int) time >> 11;
        auto minutes   = (int) ((time >> 5) & 63);
        auto seconds   = (int) ((time & 31) << 1);

        return { year, month, day, hours, minutes, seconds };
    }