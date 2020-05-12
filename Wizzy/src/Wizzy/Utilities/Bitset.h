#pragma once

WZ_NS_BEGIN(Wizzy);

#define BIT64(x) (1ULL << x)

class Bitset
{
public:
    inline Bitset()
    {

    }
    inline void Set(u32 bit, bool value)
    {
        int setIndex = bit / 64;
        if (setIndex >= m_sets.size())
        {
            m_sets.resize(setIndex + 1);
        }
        if (value)
        {
            m_sets[setIndex] |= BIT64(bit % 64);
        }
        else 
        {
            m_sets[setIndex] &= ~BIT64(bit % 64);
        }
    }

    inline bool Get(u32 bit) const
    {
        int setIndex = bit / 64;
        if (setIndex >= m_sets.size())
        {
            return false;
        }
        return m_sets.at(setIndex) & BIT64(bit % 64);
    }

    // TODO: friend operators (#OPTIMIZE)
    inline Bitset operator&(const Bitset& rhs)
    {
        Bitset lhs = *this;
        for (int i = 0; i < std::min(lhs.m_sets.size(), rhs.m_sets.size()); i++)
        {
            lhs.m_sets[i] &= rhs.m_sets[i];
        }
        return lhs;
    }

    inline Bitset operator|(const Bitset& rhs)
    {
        Bitset lhs = *this;
        for (int i = 0; i < std::min(lhs.m_sets.size(), rhs.m_sets.size()); i++)
        {
            lhs.m_sets[i] |= rhs.m_sets[i];
        }
        return lhs;
    }

    inline bool operator==(const Bitset& rhs)
    {
        for (int i = 0; i < std::max(m_sets.size(), rhs.m_sets.size()); i++)
        {
            if ((i < m_sets.size() && i < rhs.m_sets.size() && m_sets[i] != rhs.m_sets[i]) 
                || (i >= m_sets.size() && rhs.m_sets[i] != 0)
                || (i >= rhs.m_sets.size() && m_sets[i] != 0))
            {
                return false;
            }
        }
        return true;
    }

    inline bool operator!=(const Bitset& rhs)
    {
        return !(*this == rhs);
    }

private:
    std::vector<s64> m_sets;
};

WZ_NS_END;