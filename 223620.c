    int RemoteIo::seek(int64 offset, Position pos)
    {
        assert(p_->isMalloced_);
        int64 newIdx = 0;

        switch (pos) {
            case BasicIo::cur:
                newIdx = static_cast<int64>(p_->idx_) + offset;
                break;
            case BasicIo::beg:
                newIdx = offset;
                break;
            case BasicIo::end:
                newIdx = static_cast<int64>(p_->size_) + offset;
                break;
        }

        // #1198.  Don't return 1 when asked to seek past EOF.  Stay calm and set eof_
        // if (newIdx < 0 || newIdx > (long) p_->size_) return 1;
        p_->idx_ = static_cast<size_t>(newIdx);
        p_->eof_ = newIdx > static_cast<int64>(p_->size_);
        if (p_->idx_ > p_->size_)
          p_->idx_ = p_->size_;
        return 0;
    }