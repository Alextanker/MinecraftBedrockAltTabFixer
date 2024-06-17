#include <iostream>
#include <Windows.h>
#include <appmodel.h>
#include <shlobj.h>
#include <vector>
#include <algorithm>
#include <cctype>

std::wstring PackageFullNameFromFamilyName(std::wstring familyName)
{
	std::wstring fullName;
	UINT32 count = 0;
	UINT32 length = 0;

	LONG status = FindPackagesByPackageFamily(familyName.c_str(), PACKAGE_FILTER_HEAD, &count, nullptr, &length, nullptr, nullptr);
	if (status == ERROR_SUCCESS || status != ERROR_INSUFFICIENT_BUFFER)
		return fullName;

	PWSTR* fullNames = (PWSTR*)malloc(count * sizeof(*fullNames));
	PWSTR buffer = (PWSTR)malloc(length * sizeof(WCHAR));
	UINT32* properties = (UINT32*)malloc(count * sizeof(*properties));


	if (buffer == nullptr || fullNames == nullptr || properties == nullptr)
		goto Cleanup;



	status = FindPackagesByPackageFamily(familyName.c_str(), PACKAGE_FILTER_HEAD, &count, fullNames, &length, buffer, properties);
	if (status != ERROR_SUCCESS)
		goto Cleanup;
	else
		fullName = std::wstring(fullNames[0]);

	Cleanup:
	if (properties != nullptr)
		free(properties);
	if (buffer != nullptr)
		free(buffer);
	if (fullNames != nullptr)
		free(fullNames);

	return fullName;
}

void AltTabFix()
{
	CoInitialize(NULL);
	IPackageDebugSettings* pPackageDebugSettings = NULL;
	CoCreateInstance(CLSID_PackageDebugSettings, NULL, CLSCTX_INPROC_SERVER, IID_IPackageDebugSettings, (LPVOID*)&pPackageDebugSettings);

	UINT32 count = 0, bufferLength = 0;

	std::wstring packageFamilyNameW = L"Microsoft.MinecraftUWP_8wekyb3d8bbwe";

	pPackageDebugSettings->EnableDebugging(packageFamilyNameW.c_str(), NULL, NULL);

	std::wstring packageFullNameW = PackageFullNameFromFamilyName(packageFamilyNameW);

	pPackageDebugSettings->EnableDebugging(packageFullNameW.c_str(), NULL, NULL);

}

std::string GetExecutablePath() {
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	return std::string(path);
}

bool CopyToStartup(const std::string& srcPath) {
	char startupPath[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_STARTUP, NULL, 0, startupPath))) {
		std::string destPath = std::string(startupPath) + "\\" + "MinecraftBedrockAltTabFixer.exe";

		if (CopyFileA(srcPath.c_str(), destPath.c_str(), FALSE)) {
			return true;
		}
		else {
			std::cerr << "Failed to copy file: " << GetLastError() << std::endl;
		}
	}
	else {
		std::cerr << "Failed to get startup folder path: " << GetLastError() << std::endl;
	}

	return false;
}

bool IsInStartup() {
	char startupPath[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_STARTUP, NULL, 0, startupPath))) {
		std::string destPath = std::string(startupPath) + "\\" + "MinecraftBedrockAltTabFixer.exe";

		std::string exePath = GetExecutablePath();

		return _stricmp(destPath.c_str(), exePath.c_str()) == 0;
	}
	else {
		std::cerr << "Failed to get startup folder path: " << GetLastError() << std::endl;
	}

	return false;
}

std::string ToLower(const std::string& str) {
	std::string lowerStr = str;
	std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
	return lowerStr;
}

int main()
{
	if (IsInStartup())
	{
		AltTabFix();
		return 0;
	}

	std::cout << "Minecraft bedrock alt tab fixer!\n";
	AltTabFix();
	std::cout << "Fixed!\n";
back:
	std::cout << "Add to startup? (YES) (NO)\n";
	std::string answer;
	std::cin >> answer;
	if (ToLower(answer) == "yes")
	{
		if (CopyToStartup(GetExecutablePath()))
		{
			std::cout << "Added to startup!\n";
		}
		else
		{
			std::cout << "Failed!\n";
			Sleep(5000);
		}
		return 0;
	}
	else if (ToLower(answer) == "no")
	{
		return 0;
	}
	goto back;
}

