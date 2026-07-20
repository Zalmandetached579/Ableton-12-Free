#include "InstallerCore.h"
#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <fstream>

namespace AbletonSuite {

static bool DirExists(const std::wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

static bool CreateDirDeep(const std::wstring& path) {
    if (DirExists(path)) return true;
    return SHCreateDirectoryExW(nullptr, path.c_str(), nullptr) == ERROR_SUCCESS;
}

static std::wstring DefaultInstallPath() {
    PWSTR raw = nullptr;
    std::wstring result;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, nullptr, &raw))) {
        result = raw;
        CoTaskMemFree(raw);
        result += L"\\Ableton\\Live 12 Suite";
    }
    return result;
}

InstallResult InstallerCore::DetectExistingInstall(std::wstring& outPath) {
    // Probe common install locations for an existing Ableton Live copy.
    const wchar_t* candidates[] = {
        L"C:\\Program Files\\Ableton\\Live 12 Suite",
        L"C:\\Program Files\\Ableton\\Live 11 Suite",
        L"C:\\Program Files (x86)\\Ableton\\Live 12 Suite",
    };
    for (const auto& c : candidates) {
        if (DirExists(c)) {
            outPath = c;
            return InstallResult::Found;
        }
    }
    outPath = DefaultInstallPath();
    return InstallResult::NotFound;
}

InstallResult InstallerCore::ExtractArchive(const std::wstring& archivePath,
                                            const std::wstring& destDir,
                                            ProgressCallback onProgress)
{
    if (!DirExists(destDir)) {
        if (!CreateDirDeep(destDir)) {
            return InstallResult::Failed;
        }
    }
    // Placeholder for archive extraction hook.
    // Real implementation shells out to a bundled extractor or uses a zip lib.
    std::ifstream src(archivePath, std::ios::binary);
    if (!src.is_open()) {
        return InstallResult::Failed;
    }
    if (onProgress) onProgress(0, 100);
    // simulate progress
    for (int i = 1; i <= 100; ++i) {
        if (onProgress) onProgress(i, 100);
    }
    return InstallResult::Ok;
}

InstallResult InstallerCore::RegisterShortcuts(const std::wstring& installDir) {
    std::wstring exe = installDir + L"\\Live 12 Suite.exe";
    if (!DirExists(installDir)) {
        return InstallResult::Failed;
    }
    // Desktop shortcut
    PWSTR desktopRaw = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desktopRaw))) {
        std::wstring desktop = desktopRaw;
        CoTaskMemFree(desktopRaw);
        std::wstring lnk = desktop + L"\\Ableton Live 12 Suite.lnk";
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        IShellLinkW* psl = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                       IID_IShellLinkW, (void**)&psl))) {
            psl->SetPath(exe.c_str());
            psl->SetDescription(L"Ableton Live 12 Suite");
            psl->SetWorkingDirectory(installDir.c_str());
            IPersistFile* ppf = nullptr;
            if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (void**)&ppf))) {
                ppf->Save(lnk.c_str(), TRUE);
                ppf->Release();
            }
            psl->Release();
        }
        if (SUCCEEDED(hr)) CoUninitialize();
    }
    return InstallResult::Ok;
}

}  // namespace AbletonSuite
