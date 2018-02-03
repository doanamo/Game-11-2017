#include "Precompiled.hpp"
#include "Common/Build.hpp"
#include "BuildVersion.hpp"

namespace
{
    std::string workingDir = "";
    std::string sourceDir = "";
}

void Build::Initialize(int argc, char* argv[])
{
    Log() << "Reading build info..." << LogIndent();

    // Get the executable path from the first argument.
    Verify(argc >= 1, "First launch argument is missing!");
    std::string executablePath = argv[0];

    // Read working and source directories from the build system.
    // These files are written by our CMakeLists.txt configuration file.
    workingDir = Utility::GetTextFileContent("WorkingDir.txt");
    sourceDir = Utility::GetTextFileContent("SourceDir.txt");

    // Log build information.
    if(!workingDir.empty())
    {
        LogInfo() << "Working directory: " << workingDir;
    }
    else
    {
        std::string directory = executablePath;
        directory = Utility::StringReplace(directory, "\\", "/");
        directory = directory.substr(0, directory.find_last_of('/') + 1);

        LogInfo() << "Working directory: " << directory;
    }
    
    if(!sourceDir.empty())
    {
        LogInfo() << "Source directory: " << sourceDir;
    }

    LogInfo() << "Build commit info: CL " << Build::ChangeList << ", " << Build::CommitHash << ", " << Build::BranchName;
    LogInfo() << "Build commit date: " << Build::CommitDate;

    // Done!
    Log() << "Done!";
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
