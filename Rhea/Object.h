#ifndef RHEA_OBJECT_H
#define RHEA_OBJECT_H

namespace Rhea
{
	/** Type info for objects
	*/
	class Type
	{
	public:
		bool is_a(char* type_name);
		Type* get_basetype() { return m_base_type; }
		void setTypeName(char* type_name);
		char* getTypeName() { return m_type_name; }
	protected:
		char* m_type_name = 0;
		Type* m_base_type = 0;

	};

	typedef void (*signal_func)();

	class Signal
	{
	public:
		char* m_name;
		signal_func m_signal;
	};

	/** Base class for all components in the Rhea system
	* contains information about type etc.
	*/
	class Object
	{
	protected:
		Type* m_type = 0;
		Signal* m_signals = 0;
	public:
		Type* get_type() { return m_type; }
	};
}
#endif // !RHEA_OBJECT_H
