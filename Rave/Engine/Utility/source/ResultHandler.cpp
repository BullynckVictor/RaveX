#include "Engine/Utility/ResultHandler.h"

rv::ResultHandler rv::resultHandler;

rv::ResultException::ResultException(const Result& result)
	:
	m_result(result)
{
	std::stringstream ss;
	Format(ss);
	m_message = ss.str();
}

rv::ResultException::ResultException(const Result& result, const std::string& message)
	:
	m_result(result)
{
	std::stringstream ss;
	Format(ss);
	ss << "\n\n" << "Message: " << message;
	m_message = ss.str();
}

const char* rv::ResultException::what() const
{
	return m_message.c_str();
}

const rv::Result& rv::ResultException::result() const
{
	return m_result;
}

void rv::ResultException::Format(std::stringstream& ss)
{
	const char* name = resultHandler.GetResultName(m_result);
	if (!name)
		name = "Unknown";

	ss << "Result Exception occurred!\n\n";
	ss << "Type: " << name << '\n';
	ss << "Severity: " << to_string(m_result.severity());
}

void rv::ResultHandler::RegisterResult(const Identifier32& result)
{
	std::lock_guard guard(nameMutex);
	nameMap[result.hash() & ~0b111] = result.name();
}

const char* rv::ResultHandler::GetResultName(const Result& result)
{
	std::lock_guard guard(nameMutex);
	u32 key = result.data() & ~0b111;
	return nameMap[key];
}

rv::ResultQueue& rv::ResultHandler::GetThreadQueue()
{
	std::lock_guard guard(queueMutex);
	return queueMap[std::this_thread::get_id()];
}

std::vector<std::reference_wrapper<std::pair<const std::thread::id, rv::ResultQueue>>> rv::ResultHandler::GetQueues()
{
	std::lock_guard guard(queueMutex);
	std::vector<std::reference_wrapper<std::pair<const std::thread::id, ResultQueue>>> queues;
	queues.reserve(queueMap.size());
	for (auto& queue : queueMap)
		queues.emplace_back(queue);
	return queues;
}

void rv::ResultHandler::Clear()
{
	{
		std::lock_guard guard(nameMutex);
		nameMap.clear();
	}
	{
		std::lock_guard guard(queueMutex);
		queueMap.clear();
	}
}

bool check(rv::Queue<rv::ResultQueue::ResultInfo>::Header* header, const rv::Flags<rv::Severity>& severity)
{
	return severity.contains(header->info.result.severity());
}

rv::ResultQueue::ResultQueue(ResultQueue&& rhs) noexcept
	:
	queue(std::move(rhs.queue))
{
}

rv::ResultQueue& rv::ResultQueue::operator=(ResultQueue&& rhs) noexcept
{
	queue = std::move(rhs.queue);
	return *this;
}

void rv::ResultQueue::PushResult(Result result, std::string&& message)
{
	ResultInfo r;
	r.message = std::move(message);
	r.result = result;
	queue.PushEntry(std::move(r));
}

rv::Queue<rv::ResultQueue::ResultInfo>::Header* rv::ResultQueue::GetResult(Flags<Severity> severity)
{
	return queue.GetHeader(check, severity);
}

rv::ResultInfo::ResultInfo(Queue<ResultQueue::ResultInfo>::Header* header)
	:
	header(header)
{
	header->prev = nullptr;
	header->next = nullptr;
}

rv::ResultInfo::ResultInfo(ResultInfo&& rhs) noexcept
	:
	header(rhs.header)
{
	rhs.header = nullptr;
}

rv::ResultInfo::~ResultInfo()
{
	if (valid())
		delete header;
}

rv::ResultInfo::operator bool() const
{
	return valid();
}

rv::ResultInfo& rv::ResultInfo::operator=(ResultInfo&& rhs) noexcept
{
	if (valid())
		delete header;
	header = rhs.header;
	rhs.header = nullptr;
	return *this;
}

bool rv::ResultInfo::valid() const
{
	return header;
}

bool rv::ResultInfo::invalid() const
{
	return !valid();
}

rv::Result& rv::ResultInfo::result()
{
	return header->info.result;
}

const rv::Result& rv::ResultInfo::result() const
{
	return header->info.result;
}

std::string& rv::ResultInfo::message()
{
	return header->info.message;
}

const std::string& rv::ResultInfo::message() const
{
	return header->info.message;
}

std::string& rv::ResultInfo::description()
{
	return header->info.info;
}

const std::string& rv::ResultInfo::description() const
{
	return header->info.info;
}