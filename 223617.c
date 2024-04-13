    int64 RemoteIo::tell() const
    {
        return static_cast<int64>(p_->idx_);
    }