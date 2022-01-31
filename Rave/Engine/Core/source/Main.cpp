#include "Engine/Core/Main.h"
#include "Engine/Core/Windows.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Types.h"
#include "Engine/Utility/ResultHandler.h"

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

void clean()
{
	resultHandler.Clear();
}

struct AutoCleaner
{
	~AutoCleaner() { clean(); }
};

void message_box(const char* title, const char* message, uint icon = MB_ICONERROR)
{
	int response = MessageBox(nullptr, message, title, icon | MB_RETRYCANCEL);

	if constexpr (resultHandler.enabled)
	if (response == IDRETRY)
	{
		std::stringstream ss;

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
				ss << "Main thread:\t" << results.size() << " results\n";
			else
				ss << "Thread 0x" << std::hex << thread << std::dec << ":\t" << results.size() << " results\n";


			for (const auto& result : results)
			{
				const auto& res = result.result();
				const char* name = resultHandler.GetResultName(res);
				if (!name)
					name = "Unknown";

				ss << "\nResult " << &result - results.data() + 1 << ":\n";
				ss << "Type:\t\t" << name << '\n';
				ss << "Severity:\t\t" << to_string(res.severity()) << '\n';
				if (!result.description().empty())
					ss << result.description() << '\n';
				if (!result.message().empty())
					ss << "Message:\n" << result.message() << '\n';

				if (maxSeverity < res.severity())
					maxSeverity = res.severity();
			}

			ss << "\n\n";
		}

		if (!queues.empty())
			MessageBox(nullptr, ss.str().c_str(), "Queued Result Information", to_icon(maxSeverity) | MB_OK);
	}
}

void message_box(const std::exception& exception)
{
	message_box("Standard Exception", exception.what());
}

void message_box(const ResultException& exception)
{
	message_box("Rave Exception", exception.what(), to_icon(exception.result().severity()));
}

void message_box(const Result& result)
{
	message_box(ResultException(result));
}

int main()
{
	try
	{
		AutoCleaner cleaner;
		resultHandler.RegisterResult(global_result);
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
		message_box("Unknown Error", "An unknown exception has occurred and the program has failed.\n\nPlease contact the developers");
	}
	return EXIT_FAILURE;
}