#pragma once
#include <string>
#include <functional>

namespace AbletonSuite {

enum class InstallResult {
    Ok,
    NotFound,
    Failed
};

using ProgressCallback = std::function<void(int current, int total)>;

class InstallerCore {
public:
    // Probe for an existing Ableton Live install. On NotFound, outPath is set to the default.
    static InstallResult DetectExistingInstall(std::wstring& outPath);

    // Extract the setup archive into destDir. Calls onProgress during the operation.
    static InstallResult ExtractArchive(const std::wstring& archivePath,
                                        const std::wstring& destDir,
                                        ProgressCallback onProgress = nullptr);

    // Create desktop/start-menu shortcuts pointing at the installed executable.
    static InstallResult RegisterShortcuts(const std::wstring& installDir);
};

}  // namespace AbletonSuite
