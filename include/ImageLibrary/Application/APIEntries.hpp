#pragma
#include <string>
#include <filesystem>
#include <iostream>

namespace ImageLibrary {
	class BaseAPIEntry
	{
	protected:
		BaseAPIEntry() = default;

	public:
		std::string identifier;

		virtual BaseAPIEntry *Copy() = 0;

		virtual ~BaseAPIEntry() = default;
	};

	class StaticAPIEntry : public BaseAPIEntry
	{
	public:
		StaticAPIEntry() = default;
		StaticAPIEntry(const std::string& _identifier, const std::string& _mimeType, const std::filesystem::path& _filePath) : filePath(_filePath) {
			identifier = _identifier;
			mimeType = _mimeType;
		};

		StaticAPIEntry* Copy() {
			auto *entry = new StaticAPIEntry();
			entry->filePath = filePath;
			entry->identifier = identifier;
			entry->mimeType = mimeType;
			return entry;
		};

		std::filesystem::path filePath;
		std::string mimeType;
	};

	class DynamicAPIEntry : public BaseAPIEntry
	{
	public:
		DynamicAPIEntry() = default;
		DynamicAPIEntry(const std::string& _identifier, const std::string& _mimeType, const std::filesystem::path& _folderPath) : folderPath(_folderPath) {
			identifier = _identifier;
			mimeType = _mimeType;
		};

		DynamicAPIEntry* Copy() {
			auto* entry = new DynamicAPIEntry();
			entry->folderPath = folderPath;
			entry->identifier = identifier;
			entry->mimeType = mimeType;
			return entry;
		};

		std::filesystem::path folderPath;
		std::string mimeType;
	};

	class ExecutableAPIEntry : public BaseAPIEntry
	{
	public:
		ExecutableAPIEntry() = default;
		ExecutableAPIEntry(const std::string& _identifier, const std::string& _mimeType, const std::filesystem::path& _executablePath) : executablePath(_executablePath) {
			identifier = _identifier;
		};

		ExecutableAPIEntry* Copy() {
			auto* entry = new ExecutableAPIEntry();
			entry->executablePath = executablePath;
			entry->identifier = identifier;
			return entry;
		};

		std::filesystem::path executablePath;
	};

	class APIEntry {
	private:
		BaseAPIEntry* m_entry;
	public:
		APIEntry(const StaticAPIEntry& _entry);
		APIEntry(const DynamicAPIEntry& _entry);
		APIEntry(const ExecutableAPIEntry& _entry);

		APIEntry(const APIEntry &_entry);
		APIEntry(APIEntry&& _entry);
		APIEntry& operator=(const APIEntry& _entry);
		APIEntry& operator=(APIEntry&& _entry);

		~APIEntry();

		BaseAPIEntry* const Entry() { return m_entry; };
		const BaseAPIEntry* const Entry() const { return m_entry; };
	};
}