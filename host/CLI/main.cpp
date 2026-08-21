#include "../host.h"
#include "../hookcode.h"
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <string.h>
#include <thread>

namespace
{
	std::mutex outputMutex;

	bool IsSentencePunctuation(wchar_t ch)
	{
		return ch == L'\x3002' || ch == L'\xff01' || ch == L'\xff1f' ||
			ch == L'\xff0c' || ch == L'\x3001' || ch == L'\xff1b' ||
			ch == L'\xff1a' || ch == L'.' || ch == L'!' || ch == L'?' ||
			ch == L',' || ch == L';' || ch == L':';
	}

	bool RemoveRepeatedUnit(std::wstring& text, size_t unitLength, int minimumRepeats)
	{
		if (unitLength == 0 || unitLength * minimumRepeats > text.size()) return false;
		std::wstring unit(text, 0, unitLength);
		for (size_t index = unitLength; index < text.size(); ++index)
			if (text[index] != unit[index % unitLength]) return false;
		text = std::move(unit);
		return true;
	}

	void NormalizeUnityOutput(const HookParam& hp, std::wstring& text)
	{
		if (_wcsicmp(hp.module, L"GameAssembly.dll") != 0) return;
		int cjk = 0;
		for (auto ch : text)
			if ((ch >= L'\x3400' && ch <= L'\x9fff') ||
				(ch >= L'\x3040' && ch <= L'\x30ff') ||
				(ch >= L'\xac00' && ch <= L'\xd7af')) ++cjk;
		if (cjk < 4) return;

		for (size_t index = 0; index < text.size() && index < 300; ++index)
			if (IsSentencePunctuation(text[index]) && RemoveRepeatedUnit(text, index + 1, 2))
				return;
		for (size_t length = 4; length <= text.size() / 2 && length <= 300; ++length)
			if (RemoveRepeatedUnit(text, length, 3)) return;
	}

	std::wstring SanitizeProtocolField(std::wstring value)
	{
		for (auto& ch : value)
			if (ch == L'\t' || ch == L'\r' || ch == L'\n') ch = L' ';
		return value;
	}

	void WriteSearchState(const wchar_t* state, DWORD processId)
	{
		std::lock_guard<std::mutex> lock(outputMutex);
		wprintf_s(L"NEKO_SEARCH_%s\t%u\n", state, processId);
		fflush(stdout);
	}

	void WriteSearchResult(DWORD processId, HookParam hp, std::wstring text)
	{
		std::wstring hookCode = SanitizeProtocolField(HookCode::Generate(hp, processId));
		NormalizeUnityOutput(hp, text);
		text = SanitizeProtocolField(std::move(text));
		std::lock_guard<std::mutex> lock(outputMutex);
		wprintf_s(L"NEKO_SEARCH_RESULT\t%u\t%s\t%s\n", processId, hookCode.c_str(), text.c_str());
		fflush(stdout);
	}
}

int main(int argc, char* argv[])
{
	if (argc == 2 && (_stricmp(argv[1], "--neko-info") == 0 || _stricmp(argv[1], "--version") == 0))
	{
		printf("{\"name\":\"NekoTextractor\",\"version\":\"%s\",\"upstream\":\"Textractor\",\"capabilities\":[\"textractor_cli_protocol\",\"hook_search_protocol\",\"engine_auto_hook\",\"unity_il2cpp_preset\",\"the_lamenting_geese_hook\"]}\n", VERSION);
		fflush(stdout);
		return 0;
	}
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
	wprintf_s(L"Usage: {'attach'|'detach'|'search'|hookcode} -Pprocessid\n");
	fflush(stdout);
	Host::Start([](auto) {}, [](auto) {}, [](auto&) {}, [](auto&) {}, [](TextThread& thread, std::wstring& output)
	{
		NormalizeUnityOutput(thread.hp, output);
		std::lock_guard<std::mutex> lock(outputMutex);
		wprintf_s(L"[%I64X:%I32X:%I64X:%I64X:%I64X:%s:%s] %s\n",
			thread.handle,
			thread.tp.processId,
			thread.tp.addr,
			thread.tp.ctx,
			thread.tp.ctx2,
			thread.name.c_str(),
			HookCode::Generate(thread.hp, thread.tp.processId).c_str(),
			output.c_str()
		);
		fflush(stdout);
		return false;
	});
	wchar_t input[500] = {};
	while (fgetws(input, 500, stdin))
	{
		wchar_t command[500] = {};
		DWORD processId = 0;
		if (swscanf(input, L"%499s -P%d", command, &processId) != 2) ExitProcess(0);
		if (_wcsicmp(command, L"attach") == 0) Host::InjectProcess(processId);
		else if (_wcsicmp(command, L"detach") == 0) Host::DetachProcess(processId);
		else if (_wcsicmp(command, L"search") == 0)
		{
			SearchParam sp = {};
			sp.length = 0;
			sp.searchTime = 5000;
			sp.maxRecords = 20000;
			try
			{
				WriteSearchState(L"STARTED", processId);
				Host::FindHooks(processId, sp, [processId](HookParam hp, std::wstring text)
				{
					WriteSearchResult(processId, hp, std::move(text));
				});
				std::thread([processId, searchTime = sp.searchTime]
				{
					Sleep(searchTime + 750);
					WriteSearchState(L"DONE", processId);
				}).detach();
			}
			catch (std::out_of_range&)
			{
				WriteSearchState(L"ERROR", processId);
			}
		}
		else if (auto hp = HookCode::Parse(command)) Host::InsertHook(processId, hp.value());
		else ExitProcess(0);
	}
	ExitProcess(0);
}
