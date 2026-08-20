#include "extension.h"

namespace
{
	bool StartsWith(const wchar_t* text, const wchar_t* prefix)
	{
		return text && wcsncmp(text, prefix, wcslen(prefix)) == 0;
	}

	bool IsOpeningQuote(wchar_t ch)
	{
		return ch == L'\x300c' || ch == L'\x300e' || ch == L'\x3010' || ch == L'\xff08' || ch == L'(' || ch == L'\x201c';
	}

	bool RemoveRepeatedOpenQuote(std::wstring& sentence)
	{
		if (sentence.size() < 2 || sentence[0] != sentence[1] || !IsOpeningQuote(sentence[0])) return false;
		auto firstDifferent = sentence.find_first_not_of(sentence[0]);
		if (firstDifferent == std::wstring::npos) return false;
		sentence.erase(1, firstDifferent - 1);
		return true;
	}

	bool RemoveRepeatedPrefix(std::wstring& sentence)
	{
		auto timeout = GetTickCount64() + 30'000;
		auto data = std::make_unique<wchar_t[]>(sentence.size() + 1);
		wcscpy_s(data.get(), sentence.size() + 1, sentence.c_str());
		wchar_t* dataEnd = data.get() + sentence.size();
		int skip = 0, count = 0;
		for (wchar_t* end = dataEnd; end - data.get() > skip && GetTickCount64() < timeout; --end)
		{
			std::swap(*end, *dataEnd);
			int junkLength = end - data.get() - skip;
			auto junkFound = wcsstr(sentence.c_str() + skip + junkLength, data.get() + skip);
			std::swap(*end, *dataEnd);
			if (junkFound)
			{
				if (count && junkLength < min(skip / count, 4)) break;
				skip += junkLength;
				count += 1;
				end = dataEnd;
			}
		}
		if (count && skip / count >= 3)
		{
			std::wstring candidate = data.get() + skip;
			Trim(candidate);
			if (!candidate.empty() && IsOpeningQuote(candidate.front()))
			{
				sentence = candidate;
				return true;
			}
		}
		return false;
	}
}

bool ProcessSentence(std::wstring& sentence, SentenceInfo sentenceInfo)
{
	if (sentenceInfo["text number"] == 0) return false;
	if (!StartsWith((const wchar_t*)sentenceInfo["text name"], L"KiriKiri")) return false;

	bool changed = false;
	changed |= RemoveRepeatedPrefix(sentence);
	changed |= RemoveRepeatedOpenQuote(sentence);
	return changed;
}

TEST(
	{
		InfoForExtension kiriKiri[] = { { "text number", 1 }, { "text name", (int64_t)L"KiriKiriZ" }, {} };
		InfoForExtension other[] = { { "text number", 1 }, { "text name", (int64_t)L"Other" }, {} };

		std::wstring repeated = L"\x300c""wake\x300d\x300c""wake\x300d";
		std::wstring repeatedOpenQuote = L"\x300c\x300c""sorry\x300d";
		std::wstring normalRepeat = L"wait, wait please.";
		std::wstring otherEngine = L"\x300c""wake\x300d\x300c""wake\x300d";

		ProcessSentence(repeated, { kiriKiri });
		ProcessSentence(repeatedOpenQuote, { kiriKiri });
		ProcessSentence(normalRepeat, { kiriKiri });
		ProcessSentence(otherEngine, { other });

		assert(repeated == L"\x300c""wake\x300d");
		assert(repeatedOpenQuote == L"\x300c""sorry\x300d");
		assert(normalRepeat == L"wait, wait please.");
		assert(otherEngine == L"\x300c""wake\x300d\x300c""wake\x300d");
	}
);
