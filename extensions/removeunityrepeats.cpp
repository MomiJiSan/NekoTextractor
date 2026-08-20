#include "extension.h"

namespace
{
	bool EqualsIgnoreCase(const wchar_t* a, const wchar_t* b)
	{
		return a && b && _wcsicmp(a, b) == 0;
	}

	void TrimInPlace(std::wstring& text)
	{
		while (!text.empty() && iswspace(text.front())) text.erase(text.begin());
		while (!text.empty() && iswspace(text.back())) text.pop_back();
	}

	bool IsCjk(wchar_t ch)
	{
		return ch >= L'\x4e00' && ch <= L'\x9fff';
	}

	bool IsSentencePunctuation(wchar_t ch)
	{
		return ch == L'\x3002' || ch == L'\xff01' || ch == L'\xff1f' || ch == L'\xff0c' || ch == L'\x3001' || ch == L'\xff1b' || ch == L'\xff1a' ||
			ch == L'.' || ch == L'!' || ch == L'?' || ch == L',' || ch == L';' || ch == L':';
	}

	bool IsSingleCharacterSpam(const std::wstring& sentence)
	{
		wchar_t repeated = 0;
		int count = 0;
		for (auto ch : sentence)
		{
			if (iswspace(ch)) continue;
			if (!count) repeated = ch;
			else if (ch != repeated) return false;
			++count;
		}
		return count >= 12;
	}

	bool RemoveRepeatedUnit(std::wstring& sentence, size_t unitLength, int minimumRepeats)
	{
		if (unitLength == 0 || unitLength * minimumRepeats > sentence.size()) return false;
		std::wstring unit(sentence, 0, unitLength);
		int repeats = 1;
		size_t pos = unitLength;
		while (pos + unitLength <= sentence.size() && sentence.compare(pos, unitLength, unit) == 0)
		{
			++repeats;
			pos += unitLength;
		}
		if (repeats < minimumRepeats) return false;
		if (pos < sentence.size() && unit.compare(0, sentence.size() - pos, sentence, pos, sentence.size() - pos) != 0) return false;
		sentence = std::move(unit);
		return true;
	}

	bool RemoveRepeatedSentence(std::wstring& sentence)
	{
		int cjk = 0;
		for (auto ch : sentence) if (IsCjk(ch)) ++cjk;
		if (cjk < 4) return false;

		for (size_t i = 0; i < sentence.size() && i < 300; ++i)
			if (IsSentencePunctuation(sentence[i]) && RemoveRepeatedUnit(sentence, i + 1, 2))
				return true;

		for (size_t length = 4; length <= sentence.size() / 2 && length <= 300; ++length)
			if (RemoveRepeatedUnit(sentence, length, 3))
				return true;
		return false;
	}
}

bool ProcessSentence(std::wstring& sentence, SentenceInfo sentenceInfo)
{
	if (sentenceInfo["text number"] == 0) return false;
	if (!EqualsIgnoreCase((const wchar_t*)sentenceInfo["hook module"], L"GameAssembly.dll")) return false;

	TrimInPlace(sentence);
	if (sentence.empty()) return true;
	if (IsSingleCharacterSpam(sentence)) Skip();
	return RemoveRepeatedSentence(sentence);
}

TEST(
	{
		InfoForExtension unity[] = { { "text number", 1 }, { "hook module", (int64_t)L"GameAssembly.dll" }, {} };
		InfoForExtension other[] = { { "text number", 1 }, { "hook module", (int64_t)L"Other.dll" }, {} };

		std::wstring repeated = L"\x6b64\x75c5\x4ee4\x6211\x7cbe\x795e\x840e\x9761\x3001\x8bb0\x5fc6\x8870\x9000\x3002\x6b64\x75c5\x4ee4\x6211\x7cbe\x795e\x840e\x9761\x3001\x8bb0\x5fc6\x8870\x9000\x3002";
		std::wstring clean = L"\x6b64\x75c5\x4ee4\x6211\x7cbe\x795e\x840e\x9761\x3001\x8bb0\x5fc6\x8870\x9000\x3002";
		std::wstring spam = L"\x021e\x021e\x021e\x021e\x021e\x021e\x021e\x021e\x021e\x021e\x021e\x021e";
		std::wstring otherEngine = repeated;

		ProcessSentence(repeated, { unity });
		assert(repeated == clean);
		try { ProcessSentence(spam, { unity }); assert(false); } catch (SKIP) {}
		ProcessSentence(otherEngine, { other });
		assert(otherEngine != clean);
	}
);
