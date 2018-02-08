#ifndef _ANYTYPE_
#define _ANYTYPE_

#include <typeinfo>  // typeid
#include <algorithm> // std::swap

class any
{
	public:
		any() : content(0)
		{
		}

		~any()
		{
			delete content;
		}

		const std::type_info &type_info() const
		{
			return content ? content->type_info() : typeid(void);
		}

		any(const any &other)
			: content(other.content ? other.content->clone() : 0)
		{
		}

		template<typename T> any(const T &value)
			: content(new holder<T>(value))
		{
		}

		any &swap(any &rhs)
		{
			std::swap(content, rhs.content);
			return *this;
		}

		any &operator=(const any &rhs)
		{
			any a(rhs);
			return swap(a);
		}

		template<typename T> any &operator=(const T &rhs)
		{
			return swap(any(rhs));
		}

		operator const void *() const
		{
			return content;
		}

		template<typename T>
		bool copy_to(T &value) const
		{
			const T *copyable = to_ptr<T>();
			if(copyable)
				value = *copyable;
			return copyable;
		}

		template<typename T>
		const T *to_ptr() const
		{
			return type_info() == typeid(T)
				? &static_cast<holder<T> *>(content)->held
				: 0;
		}

		bool empty() const
		{
			return content == 0;
		}

		template<typename T>
		static T as(const any &operand)
		{
			const T *result = operand.to_ptr<T>();
			return result ? *result : throw std::bad_cast();
		}

	private:
		class placeholder
		{
			public:
				virtual ~placeholder()
				{
				}
				virtual const std::type_info & type_info() const = 0;
				virtual placeholder *clone() const = 0;
		};

		template<typename T>
		class holder : public placeholder
		{
			public:
				holder(const T &value) : held(value)
				{
				}

				virtual const std::type_info &type_info() const
				{
					return typeid(T);
				}

				virtual placeholder *clone() const
				{
					return new holder(held);
				}

				const T held;
		};

		placeholder *content;
};

#endif
