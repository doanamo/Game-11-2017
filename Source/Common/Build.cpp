#include "Precompiled.hpp"
#include "Common/Build.hpp"
#include "BuildVersion.hpp"

namespace
{
    std::string workingDir = "";
    std::string sourceDir = "";
}

void Build::Initialize()
{
    // Read working and source directories from the build system.
    // These files are written by our CMakeLists.txt configuration file.
    workingDir = Utility::GetTextFileContent("WorkingDir.txt");
    sourceDir = Utility::GetTextFileContent("SourceDir.txt");

    // Log build information.
    Log() << "Working directory: " << workingDir;
    Log() << "Source directory: " << sourceDir;
    Log() << "Build commit info: CL " << Build::ChangeList << ", " << Build::CommitHash << ", " << Build::BranchName;
    Log() << "Build commit date: " << Build::CommitDate;
}

std::string Build::GetWorkingDir()
{
    return workingDir;
}

std::string Build::GetSourceDir()
{
    return sourceDir;
}

std::string Build::GetChangeList()
{
    return Build::ChangeList;
}

std::string Build::GetCommitHash()
{
    return Build::CommitHash;
}

std::string Build::GetCommitDate()
{
    return Build::CommitDate;
}

std::string Build::GetBranchName()
{
    return Build::BranchName;
}
