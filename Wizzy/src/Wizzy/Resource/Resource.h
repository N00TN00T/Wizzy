#pragma once

#include "Wizzy/PropertyLibrary.h"

namespace Wizzy {

    typedef std::vector<byte> ResData;
    typedef u32 uId;

    class Resource 
    {
    public:
        
        struct Handle 
        {
            Handle() : id(WZ_NULL_RESOURCE_HANDLE) {}
            Handle(uId id) : id(id) {}
            Handle(const Handle&) = default;
            operator uId&() { return id; }

            friend bool operator >(const Handle& a, const Handle& b)
            {
                return a.id > b.id;
            }
            friend bool operator <(const Handle& a, const Handle& b)
            {
                return a.id < b.id;
            }

            bool operator==(const Handle& other) const
            {
                return other.id == id;
            }

            uId id; 

            struct hash
            {
                size_t operator()(const Wizzy::Resource::Handle& h) const
                {
                    return h.id;
                }
            };

        };

        template <typename T>
        using HandleMap = std::unordered_map<Resource::Handle, T, Resource::Handle::hash>;

    public:
        Resource(const PropertyLibrary& props);
        virtual ~Resource() {}

        inline PropertyLibrary& GetProps() { return m_props; }

        virtual ResData Serialize() const = 0;

    protected:
        PropertyLibrary m_props;
    };
}