/*



*/

#pragma once

#include <vector>

namespace u
{
	//-----------------------------------------------------------------------------
	// helper for streaming arbitrary structures etc to a buffer
	// streamer s;
	// s << int(4);
	// s << float(4.321);
	class streamer
	{
		//
		std::vector<char> m_buffer;
		//
		size_t write(const char* ps, size_t size)
		{
			m_buffer.insert(m_buffer.end(), ps, ps + size);
			return size;
		}
	public:
		//
		streamer() {}

		// variable length
		// use std::make_pair<>(ps,size) to create *arg*
		template <typename T, typename S>
		streamer& operator<<(const std::pair<T*, S>& arg)
		{
			size_t size = arg.second;
			const char* ps = reinterpret_cast<const char*>(arg.first);
			write(ps, size);
			return (*this);
		}

		// fixed size 'thing', scalar or struct
		template <typename T>
		streamer& operator<<(const T& arg)
		{
			size_t size = sizeof(arg);
			const char* ps = reinterpret_cast<const char*>(&arg);
			write(ps, size);
			return (*this);
		}

		// for writing, return a reference or raw pointer/size
		const std::vector<char>& ref() const { return m_buffer; }
		const char* data() const { return m_buffer.data(); }
		int size() const { return static_cast<int>(m_buffer.size()); }

		// e.g. ICONHEADER* pih = streamer.get<ICONHEADER>(0);
		template <typename T>
		T* get(size_t offset) {
			T* ret = ((offset + sizeof(T)) <= m_buffer.size() ?
				(T*)(m_buffer.data() + offset) :
				nullptr);
			return ret;
		}

		// const version
		template <typename T>
		const T* get(size_t offset) const {
			T* ret = ((offset + sizeof(T)) <= m_buffer.size() ?
				(T*)(m_buffer.data() + offset) :
				nullptr);
			return ret;
		}

		// return scalar or struct
		template <typename T>
		T value(size_t offset) {
			T* ret = (T*)get<T*>(offset);
			return *ret;
		}

	};

}