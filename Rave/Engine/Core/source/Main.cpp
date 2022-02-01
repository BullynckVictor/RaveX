#include "Engine/Core/Main.h"
#include "Engine/Core/Windows.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Types.h"
#include "Engine/Utility/ResultHandler.h"
#include "Engine/Utility/String.h"
#include "Engine/Core/AutoStartupClean.h"

using namespace rv;

uint to_icon(Severity severity)
{
	switch (severity)
	{
		case RV_SEVERITY_NULL:
		case RV_SEVERITY_INFO:		return MB_ICONINFORMATION;
		case RV_SEVERITY_WARNING:	return MB_ICONWARNING;
		case RV_SEVERITY_ALL:
		case RV_SEVERITY_ERROR:		return MB_ICONERROR;
		default:						return 0;
	}
}

void message_box(const wchar_t* title, const wchar_t* message, uint icon = MB_ICONERROR)
{
	int response = MessageBox(nullptr, message, title, icon | MB_RETRYCANCEL);

	if constexpr (resultHandler.enabled)
	if (response == IDRETRY)
	{
		std::wostringstream ss;

		Severity maxSeverity = RV_SEVERITY_NULL;

		auto queues = resultHandler.GetQueues();
		for (auto& ref : queues)
		{
			std::thread::id thread = ref.get().first;
			ResultQueue& queue = ref.get().second;

			std::vector<ResultInfo> results;
			while (auto result = queue.GetResult())
				results.emplace_back(result);

			if (thread == std::this_thread::get_id())
				detail::str(ss, L"Main thread:\t", results.size(), L" results\n");
			else
				detail::str(ss, L"Thread 0x", std::hex, thread, std::dec, L":\t", results.size(), L" results\n");


			for (const auto& result : results)
			{
				const auto& res = result.result();
				const char* name = resultHandler.GetResultName(res);
				if (!name)
					name = "Unknown";

				detail::str(ss, L"\nResult ", &result - results.data() + 1, L":\n");
				detail::str(ss, L"Type:\t\t", name, L'\n');
				detail::str(ss, L"Severity:\t\t", to_wstring(res.severity()), L'\n');
				if (!result.description().empty())
					detail::str(ss, result.description(), L'\n');
				if (!result.message().empty())
					detail::str(ss, L"Message:\n", result.message(), L'\n');

				if (maxSeverity < res.severity())
					maxSeverity = res.severity();
			}

			ss << L"\n\n";
		}

		if (!queues.empty())
			MessageBox(nullptr, ss.str().c_str(), L"Queued Result Information", to_icon(maxSeverity) | MB_OK);
	}
}

void message_box(const std::exception& exception)
{
	message_box(L"Standard Exception", utf16_string(exception.what()).c_str<wchar_t>());
}

void message_box(const ResultException& exception)
{
	message_box(L"Rave Exception", exception.wide_what(), to_icon(exception.result().severity()));
}

void message_box(const Result& result)
{
	message_box(ResultException(result));
}

int main()
{
	try
	{
		AutoStartupClean startup_cleaner;
		Result result = rv_main();

		if (result.fatal())
		{
			message_box(result);
			return EXIT_FAILURE;
		}
		else
		{
			return EXIT_SUCCESS;
		}
	}
	catch (const ResultException& e)
	{
		message_box(e);
	}
	catch (const std::exception& e)
	{
		message_box(e);
	}
	catch (...)
	{
		message_box(L"Unknown Error", L"An unknown exception has occurred and the program has failed.\n\nPlease contact the developers");
	}
	return EXIT_FAILURE;
}