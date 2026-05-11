#pragma once

enum class UiLanguage
{
	English,
	Turkish,
	Spanish,
	ChineseSimplified,
	Russian,
	Indonesian,
	Italian,
	Portuguese,
	Thai,
	Korean,
	French,
};

UiLanguage CurrentUiLanguage();
const char* UiLanguageName(UiLanguage language);
UiLanguage UiLanguageFromName(const char* name);
void SetUiLanguage(UiLanguage language);
