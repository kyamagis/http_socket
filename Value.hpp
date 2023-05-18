#ifndef VALUE_HPP
# define VALUE_HPP

# include	<iostream>
# include	<fstream>
# include	<vector>

typedef enum e_status
{
	NOT_SET,
	SET,
	DEFAULT_SET
}	t_status;

template <class T>
class Value
{
	private:
		t_status	_status;
		T			_value;
	public:
		Value() : _status(NOT_SET) {};
		Value(T value) : _status(DEFAULT_SET), _value(value) {};
		~Value() {};
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
			if (this->_status == DEFAULT_SET)
				this->_value.clear();
			this->_status = SET;
			this->_value.push_back(value);
		}
		void		clearValueClass()
		{
			this->_status = NOT_SET;
			this->_value.clear();
		}

};

template <class T>
std::ostream&	operator<<(std::ostream& os, const Value<T>& obj)
{
	if (obj.getStatus() != NOT_SET)
		os << obj.getValue() << ((obj.getStatus() == DEFAULT_SET) ? " (default set)" : " (set)");
	else
		os << "(not set)";
	return (os);
}

template <class T>
std::ostream&	operator<<(std::ostream& os, const Value<std::vector<T> >& obj)
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
