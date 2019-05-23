#include "wzpch.h"

#include "Wizzy/Resources/IResource.h"
#include "Wizzy/Utils.h"

namespace Wizzy {

    u32 IResource::s_idCounter(0);

    IResource::IResource(const string& sourceFile)
            : m_sourceFile(File::to_absolute(sourceFile)) {}

}
