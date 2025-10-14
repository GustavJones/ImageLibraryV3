#include "ImageLibrary/Application/APIEntries.hpp"

namespace ImageLibrary {
  APIEntry::APIEntry(const StaticAPIEntry& _entry) {
    m_entry = new StaticAPIEntry(_entry);
  }

  APIEntry::APIEntry(const DynamicAPIEntry& _entry) {
    m_entry = new DynamicAPIEntry(_entry);
  }

  APIEntry::APIEntry(const ExecutableAPIEntry& _entry) {
    m_entry = new ExecutableAPIEntry(_entry);
  }

  APIEntry::APIEntry(const APIEntry& _entry) : m_entry(_entry.m_entry ? _entry.m_entry->Copy() : nullptr) {
  
  }

  APIEntry::APIEntry(APIEntry&& _entry) : m_entry(_entry.m_entry) {
    _entry.m_entry = nullptr;
  }

  APIEntry& APIEntry::operator=(const APIEntry& _entry) {
    if (this == &_entry)
    {
      return *this;
    }

    delete m_entry;

    if (_entry.m_entry)
    {
      this->m_entry = _entry.m_entry->Copy();
    }
    else
    {
      this->m_entry = nullptr;
    }

    return *this;
  }

  APIEntry& APIEntry::operator=(APIEntry&& _entry) {
    if (this == &_entry)
    {
      return *this;
    }

    delete m_entry;

    m_entry = _entry.m_entry;
    _entry.m_entry = nullptr;

    return *this;
  }

  APIEntry::~APIEntry() {
    delete m_entry;
  }
}