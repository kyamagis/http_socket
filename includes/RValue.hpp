#ifndef RVALUE_HPP
# define RVALUE_HPP

# include	<iostream>
# include	<fstream>
# include	<vector>

# include	"./Value.hpp"

template <class T>
class RValue
{
	private:
		t_status	_status;
		T			_value;
	public:
		RValue() : _status(NOT_SET) {};
		~RValue() {};
		T			getValue(void) const {return (this->_value);};
		t_status	getStatus() const {return (this->_status);};
		void		setValue(T value)
		{
			this->_status = SET;
			this->_value = value;
		};
		template <class U>
		void		setValue(U value)
		{
			this->_status = SET;
			this->_value = value;
		}
		template <class V>
		void		clearValueClass(V value)
		{
			this->_status = NOT_SET;
			this->_value = value;
		}

};

template <class T>
std::ostream&	operator<<(std::ostream& os, const RValue<T>& obj)
{
	if (obj.getStatus() != NOT_SET)
		os << obj.getValue() << ((obj.getStatus() == DEFAULT_SET) ? " (default set)" : " (set)");
	else
		os << "(not set)";
	return (os);
}

template <class T>
std::ostream&	operator<<(std::ostream& os, const RValue<std::vector<T> >& obj)
{
	if (obj.getStatus() != NOT_SET) {
		std::vector<T>	vector = obj.getValue();
		size_t			size = vector.size();
		os << ((obj.getStatus() == DEFAULT_SET) ? "(default set)" : "(set)") << std::endl;
		for (size_t i = 0; i < size; i++) {
			os << "[" << i + 1 << "] " << vector[i];
			if (i + 1 != size)
				os << std::endl;
		}
	} else
		os << "(not set)";
	return (os);
}

#endif
