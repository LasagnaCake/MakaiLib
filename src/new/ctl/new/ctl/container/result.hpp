#ifndef CTL_CONTAINER_RESULT_H
#define CTL_CONTAINER_RESULT_H

#include "../templates.hpp"
#include "error.hpp"
#include "nullable.hpp"
#include "function.hpp"

CTL_NAMESPACE_BEGIN

template<typename TData, typename TError>
class Result:
public SelfIdentitied<Result<TData, TError>>,
public Typed<TData>,
public Defaultable<TData, TData()> {
public:
	using Typed				= Typed<TData>;
	using SelfIdentified	= SelfIdentitied<Result<TData, TError>>;

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

	constexpr Result(SelfType const& other)		{result = other.result; state = other.state;				}
	constexpr Result(SelfType&& other) 			{result = move(other.result); state = move(other.state);	}
	constexpr Result(ConstReferenceType value)	{result.value = value; state = ResultState::RS_OK;			}
	constexpr Result(TemporaryType value)		{result.value = move(value); state = ResultState::RS_OK;	}
	constexpr Result(ErrorType const& value)	{result.error = error; state = ResultState::RS_ERROR;		}
	constexpr Result(ErrorType&& value)			{result.error = move(error); state = ResultState::RS_ERROR;	}

	constexpr SelfType& then(Procedure<ConstReferenceType> const& proc)  				{if (ok()) proc(result.value); return *this;	}
	constexpr SelfType const& then(Procedure<ConstReferenceType> const& proc) const 	{if (ok()) proc(result.value); return *this;	}

	constexpr SelfType& onError(Procedure<ErrorType const&> const& proc)				{if (!ok()) proc(result.error); return *this;	}
	constexpr SelfType const& onError(Procedure<ErrorType const&> const& proc) const	{if (!ok()) proc(result.error); return *this;	}

	constexpr SelfType& operator=(DataType const& value)	{result.value = value; state = ResultState::RS_OK; return *this;			}
	constexpr SelfType& operator=(DataType&& value)			{result.value = move(value); state = ResultState::RS_OK; return *this;		}
	constexpr SelfType& operator=(ErrorType const& error)	{result.error = error; state = ResultState::RS_ERROR; return *this;			}
	constexpr SelfType& operator=(ErrorType&& error)		{result.error = move(error); state = ResultState::RS_ERROR; return *this;	}

	constexpr bool operator==(DataType const& value) const	{if (!ok()) return false; return result.value == value;	}
	constexpr bool operator==(ErrorType const& value) const	{if (ok()) return false; return result.error == error;	}

	constexpr bool ok()	const			{return state == ResultState::RS_OK;	}
	constexpr operator bool() const		{return ok();							}
	constexpr bool operator()() const	{return ok();							}

	constexpr SelfType& operator()(Procedure<DataType const&> const& proc)				{return then(proc);		}
	constexpr SelfType const& operator()(Procedure<DataType const&> const& proc) const	{return then(proc);		}


	constexpr SelfType& operator()(Procedure<ErrorType const&> const& proc)				{return onError(proc);	}
	constexpr SelfType const& operator()(Procedure<ErrorType const&> const& proc) const	{return onError(proc);	}

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
