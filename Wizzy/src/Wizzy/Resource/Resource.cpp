#include "wzpch.h"

#include "Wizzy/Resource/Resource.h"
#include "Wizzy/Utils.h"

namespace Wizzy {

    Resource::Resource(const Flagset& flags,
                       const string& resourceType,
                       const string& extension)
            : m_flags(flags),
              m_resourceType(resourceType),
              m_extension(extension) {}

}
