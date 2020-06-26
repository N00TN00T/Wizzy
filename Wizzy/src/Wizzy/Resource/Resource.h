#pragma once

#include "Wizzy/Utilities/PropertyTable.h"


#define __HANDLE_DEF struct Handle : public ::Wizzy::Resource::Handle \
{ \
Handle() : ::Wizzy::Resource::Handle(WZ_NULL_RESOURCE_HANDLE.id) { type = typestr(*this); }\
explicit Handle(::Wizzy::uId id) : ::Wizzy::Resource::Handle(id) { type = typestr(*this); }\
operator ::Wizzy::uId& () { return id; type = typestr(*this); }\
virtual ~Handle() {  }\
Handle(const ::Wizzy::Resource::Handle& h) \
{ \
type = typestr(*this);\
    if (h.id != WZ_NULL_ID)\
    {\
        WZ_CORE_ASSERT(h.GetType() == this->type, "Invalid handle conversion from '" + h.GetType() + "' to '" + this->type + "'"); \
        id = h.id; \
    }\
    else\
        id = WZ_NULL_ID;\
}\
} \

namespace Wizzy {

    typedef std::vector<byte> ResData;
    typedef u64 uId;

    class Resource 
    {
    public:
        
        struct Handle 
        {
            Handle() : id(WZ_NULL_RESOURCE_HANDLE.id) {}
            explicit Handle(uId id) : id(id) {}
            operator uId&() { return id; }
            virtual ~Handle() {}

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

            bool operator!=(const Handle& other) const
            {
                return !(other.id == id);
            }

            uId id; 

            struct hash
            {
                size_t operator()(const Wizzy::Resource::Handle& h) const
                {
                    return h.id;
                }
            };

            template <typename T>
            inline bool Is() const
            {
                return type == typestr(T::Handle);
            }

            inline static Resource::Handle null()
            {
                static Resource::Handle h = Resource::Handle(0);
                return h;
            }

            inline const string& GetType() const { return type; }

        protected:
            string type;
        };

        template <typename T>
        using HandleMap = std::unordered_map<Resource::Handle, T, Resource::Handle::hash>;

    public:
        Resource(const PropertyTable& props);
        virtual ~Resource() {}

        inline PropertyTable& GetProps() { return m_props; }

        virtual ResData Serialize() const = 0;

    protected:
        PropertyTable m_props;

    public:
        
    };
}