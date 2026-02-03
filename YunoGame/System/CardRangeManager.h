#pragma once

struct RangeOffset
{
    int dx;
    int dy;
};

struct RangeData
{
    uint32_t rangeId;
    std::vector<RangeOffset> offsets;
};

class CardRangeManager
{
public:
    bool LoadFromCSV(const std::string& path);

    const RangeData* GetRange(uint32_t rangeId) const;
    size_t GetRangeCount() const { return m_ranges.size(); }

private:
    std::unordered_map<uint32_t, RangeData> m_ranges;
};
