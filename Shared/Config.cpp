// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================
#include <iostream>
#include <string>
#include <list>
#include <cstdio>
#include <cctype>
#include "Shared.h"

using namespace std;

BOOL bUniversalConfig = FALSE;

TTX_ConfigManager::TTX_ConfigManager()
{
	for (int i=0; i < __CONFIG_MAX__; i++)
		optTable[i] = 0;
}

TTX_ConfigManager::~TTX_ConfigManager()
{
}

DWORD TTX_ConfigManager::GetConfig(TTX_ConfigDef index)
{
	return optTable[index];
}

void TTX_ConfigManager::Config()
{
}

void TTX_ConfigManager::SetConfig(TTX_ConfigDef index, DWORD val)
{
	optTable[index] = val;
}

void TTX_ConfigManager::Load()
{

	FILE *fp = fopen(MONITOR_MISC_FILENAME, "r");
	if (fp) {
		fread(optTable, 4, __CONFIG_MAX__, fp);
		fclose(fp);
	}

}
void TTX_ConfigManager::Save()
{
	FILE *fp = fopen(MONITOR_MISC_FILENAME, "w");
	if (fp) {
		fwrite(optTable, 4, __CONFIG_MAX__, fp);
		fclose(fp);
	}
}

