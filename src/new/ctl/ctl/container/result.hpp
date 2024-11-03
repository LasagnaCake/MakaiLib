#ifndef CTL_CONTAINER_RESULT_H
#define CTL_CONTAINER_RESULT_H

#include "../templates.hpp"
#include "error.hpp"
#include "nullable.hpp"
#include "function.hpp"

CTL_NAMESPACE_BEGIN

template<typename TData, typename TError>
class Result:
public SelfIdentified<Result<TData, TError>>,
public Typed<TData>,
public Defaultable<TData> {
public:
	using Typed				= Typed<TData>;
	using SelfIdentified	= SelfIdentified<Result<TData, TError>>;

	using
		typename Typed::DataType,
		typename Typed::ConstReferenceType,
		typename Typed::ReferenceType,
		typename Typed::TemporaryType
	;

	using
		typename SelfIdentified::SelfType
	;

	typedef TError  ErrorType;

	constexpr Result(SelfType const& other)		{result = other.result; state = other.state;						}
	constexpr Result(SelfType&& other) 			{result = CTL::move(other.result); state = CTL::move(other.state);	}
	constexpr Result(ConstReferenceType value)	{result.value = value; state = ResultState::RS_OK;					}
	constexpr Result(TemporaryType value)		{result.value = CTL::move(value); state = ResultState::RS_OK;		}
	constexpr Result(ErrorType const& value)	{result.error = error; state = ResultState::RS_ERROR;				}
	constexpr Result(ErrorType&& value)			{result.error = CTL::move(error); state = ResultState::RS_ERROR;	}

	template<Type::Functional<void(ConstReferenceType)> TFunction>
	constexpr SelfType& then(TFunction const& proc)  				{if (ok()) proc(result.value); return *this;	}
	template<Type::Functional<void(ConstReferenceType)> TFunction>
	constexpr SelfType const& then(TFunction const& proc) const 	{if (ok()) proc(result.value); return *this;	}

	constexpr SelfType& onError(Procedure<ErrorType const&> const& proc)				{if (!ok()) proc(result.error); return *this;	}
	constexpr SelfType const& onError(Procedure<ErrorType const&> const& proc) const	{if (!ok()) proc(result.error); return *this;	}

	constexpr SelfType& operator=(DataType const& value)	{result.value = value; state = ResultState::RS_OK; return *this;				}
	constexpr SelfType& operator=(DataType&& value)			{result.value = CTL::move(value); state = ResultState::RS_OK; return *this;		}
	constexpr SelfType& operator=(ErrorType const& error)	{result.error = error; state = ResultState::RS_ERROR; return *this;				}
	constexpr SelfType& operator=(ErrorType&& error)		{result.error = CTL::move(error); state = ResultState::RS_ERROR; return *this;	}

	constexpr bool operator==(DataType const& value) const	{if (!ok()) return false; return result.value == value;	}
	constexpr bool operator==(ErrorType const& value) const	{if (ok()) return false; return result.error == error;	}

	constexpr bool ok()	const			{return state == ResultState::RS_OK;	}
	constexpr operator bool() const		{return ok();							}
	constexpr bool operator()() const	{return ok();							}

	template<Type::Functional<void(ConstReferenceType)> TFunction>
	constexpr SelfType& operator()(TFunction const& proc)				{return then(proc);		}
	template<Type::Functional<void(ConstReferenceType)> TFunction>
	constexpr SelfType const& operator()(TFunction const& proc) const	{return then(proc);		}


	template<Type::Functional<void(ErrorType const&)> TFunction>
	constexpr SelfType& operator()(TFunction const& proc)				{return onError(proc);	}
	template<Type::Functional<void(ErrorType const&)> TFunction>
	constexpr SelfType const& operator()(TFunction const& proc) const	{return onError(proc);	}

	constexpr Nullable<DataType>	value() const {return ok() ? result.value : nullptr;	}
	constexpr Nullable<ErrorType>	error() const {return !ok() ? result.error : nullptr;	}

private:
	enum class ResultState {
		RS_OK,
		RS_ERROR
	};

	union ResultWrapper {
		DataType	value;
		ErrorType	error;
	};

	ResultWrapper	result;
	ResultState		state;
};

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_RESULT_H
