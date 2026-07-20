#include "InstallerCore.h"
#include "LicenseManager.h"
#include <windows.h>
#include <iostream>
#include <thread>

using namespace AbletonSuite;

static void PrintProgress(int current, int total) {
    int pct = (total > 0) ? (current * 100 / total) : 0;
    std::wcout << L"\r  progress: " << pct << L"%  " << std::flush;
}

int wmain() {
    std::wcout << L"== Ableton Live 12 Suite Setup ==\n";

    std::wstring installDir;
    InstallResult detected = InstallerCore::DetectExistingInstall(installDir);
    if (detected == InstallResult::Found) {
        std::wcout << L"  existing install detected at: " << installDir << L"\n";
    } else {
        std::wcout << L"  no existing install found, using: " << installDir << L"\n";
    }

    std::wcout << L"  extracting archive...\n";
    InstallResult ex = InstallerCore::ExtractArchive(L"Ableton-12-Live-Suite.zip",
                                                     installDir, PrintProgress);
    std::wcout << L"\n";
    if (ex != InstallResult::Ok) {
        std::wcerr << L"  extraction failed.\n";
        return 1;
    }

    std::wcout << L"  registering shortcuts...\n";
    InstallerCore::RegisterShortcuts(installDir);

    std::wcout << L"  authorizing...\n";
    LicenseManager lm;
    if (lm.AuthorizeTrial()) {
        std::wcout << L"  30-day trial authorized.\n";
    } else {
        std::wcerr << L"  authorization failed.\n";
    }

    std::wcout << L"  done.\n";
    return 0;
}
