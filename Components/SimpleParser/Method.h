#ifndef SIMPLEPARSER_METHOD_H
#define SIMPLEPARSER_METHOD_H

#include <functional>

#include "Variant.h"

namespace SimpleParser
{
	//template<typename ...Args>
	//using Mth = std::function<Variant(Variant*, Args...)>;

	template<typename ...Args>
	Variant mth(Variant*, Args...) {};

	template<class T>
	void mthd(T t);

	typedef std::function<void(Variant*, Variant&)> setter;
	typedef std::function<Variant(Variant*)> getter;

	class Method
	{
	public:
		virtual Variant execute(Variant* v) {
			return execute(v, nullptr, 0);
		}
		virtual Variant execute(Variant* v, Variant** vals, int argc) = 0;

		template<class O, class T>
		static Method* bindMethod(void (O::* method)(T)) {
			MethodBasicSetter<O, T>* mth = new MethodBasicSetter<O, T>();
			mth->setMethod(method);
			return mth;
		};

		template<class R, class O>
		static Method* bindMethod(R (O::* method)()) {
			MethodBasicGetter<O, R>* mth = new MethodBasicGetter<O, R>();
			mth->setMethod(method);
			return mth;
		};

	protected:
		std::string m_name;
	};
	
	template<class O, class T>
	class MethodBasicSetter : public Method
	{
	public:
		Variant execute(Variant* v, Variant** vals, int argc) {
			m_setter(static_cast<O*>(v), static_cast<T>(*vals[0]));
			return Variant();
		}
		void setMethod(std::function<void(O*,T)> s) { m_setter = s; }
	protected:
		std::function<void(O*,T)> m_setter;
	};

	template<class O, class R>
	class MethodBasicGetter : public Method
	{
	public:
		Variant execute(Variant* v, Variant** vals, int argc) { 
			Variant var(m_method(static_cast<O*>(v)));
			return var;
		}
		void setMethod(std::function<R(O*)> method) { m_method = method; }
	protected:
		std::function<R(O*)> m_method;
	};
}

#endif // !SIMPLEPARSER_METHOD_H
