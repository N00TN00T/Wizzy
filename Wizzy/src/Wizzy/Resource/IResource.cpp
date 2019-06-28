#include "wzpch.h"

#include "Wizzy/Resource/IResource.h"
#include "Wizzy/Utils.h"

namespace Wizzy {

    u32 IResource::s_idCounter(0);

    IResource::IResource(const string& sourceFile, const string& resourceType)
            : m_sourceFile(ulib::File::to_absolute(sourceFile)),
              m_resourceType(resourceType) {}

}
