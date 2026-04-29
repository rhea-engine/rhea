/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef __ComPtr_H__
#define __ComPtr_H__

#include <winerror.h> // for HRESULT

#if !defined(ASSUME_NOEXCEPT) && defined( _MSC_VER )
#define ASSUME_NOEXCEPT throw () // use non-compliant behavior of VC++ - compile as if exceptions are not possible, performing no run-time checks
#else
#define ASSUME_NOEXCEPT // don`t define it as 'noexcept' or compliant 'throw ()' or extra try/catch frame would be generated to call std::terminate/std::unexpected
#endif

namespace hippios
{
	// Helper to hide reference count management from user
	template <typename T>
	class NoAddRefRelease : public T
	{
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();
	};

	template <typename T>
	class ComPtr
	{
		T* m_ptr;

		template <typename U> friend class ComPtr;

		void InternalAddRef() const ASSUME_NOEXCEPT
		{
			if (m_ptr)
			{
				m_ptr->AddRef();
			}
		}

		void InternalRelease() ASSUME_NOEXCEPT
		{
			if (T* temp = m_ptr)
			{
				m_ptr = 0;
				temp->Release();
			}
		}

		void InternalCopy(T* other) ASSUME_NOEXCEPT
		{
			if (m_ptr != other)
			{
				InternalRelease();
				m_ptr = other;
				InternalAddRef();
			}
		}

	public:
		typedef T Type;

	public:
		ComPtr() ASSUME_NOEXCEPT : m_ptr(0) {}

		template <typename U>
		ComPtr(U* other) ASSUME_NOEXCEPT : m_ptr(other)
		{
			InternalAddRef();
		}

		ComPtr(const ComPtr& other) ASSUME_NOEXCEPT : m_ptr(other.m_ptr)
		{
			InternalAddRef();
		}

		template <typename U>
		ComPtr(ComPtr<U> const& other) ASSUME_NOEXCEPT : m_ptr(other.m_ptr)
		{
			InternalAddRef();
		}

		~ComPtr() ASSUME_NOEXCEPT
		{
			InternalRelease();
		}

		ComPtr& operator=(T* other) ASSUME_NOEXCEPT
		{
			InternalCopy(other);
			return *this;
		}

		template <typename U>
		ComPtr& operator=(U* other) ASSUME_NOEXCEPT
		{
			InternalCopy(other);
			return *this;
		}

		ComPtr& operator=(const ComPtr& other) ASSUME_NOEXCEPT
		{
			InternalCopy(other.m_ptr);
			return *this;
		}

		template <typename U>
		ComPtr& operator=(const ComPtr<U>& other) ASSUME_NOEXCEPT
		{
			InternalCopy(other.m_ptr);
			return *this;
		}

		void Swap(ComPtr& other) ASSUME_NOEXCEPT
		{
			T* temp = m_ptr;
			m_ptr = other.m_ptr;
			other.m_ptr = temp;
		}

		explicit operator bool() const ASSUME_NOEXCEPT
		{
			return 0 != m_ptr;
		}

		T* Get() const ASSUME_NOEXCEPT
		{
			return m_ptr;
		}

		NoAddRefRelease<T>* operator->() const ASSUME_NOEXCEPT
		{
			return static_cast<NoAddRefRelease<T>*>(m_ptr);
		}

		T** GetAddressOf() ASSUME_NOEXCEPT
		{
			return &m_ptr;
		}

		T** ReleaseAndGetAddressOf() ASSUME_NOEXCEPT
		{
			InternalRelease();
			return &m_ptr;
		}

		T* Detach() ASSUME_NOEXCEPT
		{
			T* temp = m_ptr;
			m_ptr = 0;
			return temp;
		}

		void Attach(T* other) ASSUME_NOEXCEPT
		{
			InternalRelease();
			m_ptr = other;
		}

		void Reset() ASSUME_NOEXCEPT
		{
			InternalRelease();
		}

		template <typename U>
		HRESULT As(ComPtr<U>* res) const ASSUME_NOEXCEPT
		{
			return m_ptr->QueryInterface(res->ReleaseAndGetAddressOf());
		}

	};

	template<class T> void swap(ComPtr<T>& a, ComPtr<T>& b) { return a.Swap(b); }

	template<class T> bool operator==(const ComPtr<T>& a, const ComPtr<T>& b) ASSUME_NOEXCEPT { return a.Get() == b.Get(); }
	template<class T> bool operator!=(const ComPtr<T>& a, const ComPtr<T>& b) ASSUME_NOEXCEPT { return a.Get() != b.Get(); }
	template<class T> bool operator< (const ComPtr<T>& a, const ComPtr<T>& b) ASSUME_NOEXCEPT { return a.Get() < b.Get(); }
	template<class T> bool operator> (const ComPtr<T>& a, const ComPtr<T>& b) ASSUME_NOEXCEPT { return a.Get() > b.Get(); }
	template<class T> bool operator<=(const ComPtr<T>& a, const ComPtr<T>& b) ASSUME_NOEXCEPT { return a.Get() <= b.Get(); }
	template<class T> bool operator>=(const ComPtr<T>& a, const ComPtr<T>& b) ASSUME_NOEXCEPT { return a.Get() >= b.Get(); }
}
#endif /* COMPTR_H*/