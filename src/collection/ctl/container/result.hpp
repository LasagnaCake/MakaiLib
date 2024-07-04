#ifndef CTL_CONTAINER_RESULT_H
#define CTL_CONTAINER_RESULT_H

#include "../templates.hpp"
#include "error.hpp"
#include "nullable.hpp"
#include "function.hpp"

template<typename TData, typename TError>
class Result:
public SelfIdentitied<Result<TData, TError>>,
public Typed<TData>,
public Exceptionable<TError>,
public Defaultable<TData, TData()> {
	constexpr Result(SelfType const& other)		{result = other.result; state = other.state;				}
	constexpr Result(SelfType&& other) 			{result = move(other.result); state = move(other.state);	}
	constexpr Result(DataType const& value)		{result.value = value; state = ResultState::RS_OK;			}
	constexpr Result(DataType&& value)			{result.value = move(value); state = ResultState::RS_OK;	}
	constexpr Result(ErrorType const& value)	{result.error = error; state = ResultState::RS_ERROR;		}
	constexpr Result(ErrorType&& value)			{result.error = move(error); state = ResultState::RS_ERROR;	}

	constexpr Result const& then(Procedure<ErrorType const&> const& proc) const 	{if (ok()) proc(result.value); return *this;	}
	constexpr Result const& onError(Procedure<ErrorType const&> const& proc) const	{if (!ok()) proc(result.error); return *this;	}

	constexpr Result& operator=(DataType const& value)	{result.value = value; state = ResultState::RS_OK; return *this;			}
	constexpr Result& operator=(DataType&& value)		{result.value = move(value); state = ResultState::RS_OK; return *this;		}
	constexpr Result& operator=(ErrorType const& error)	{result.error = error; state = ResultState::RS_ERROR; return *this;			}
	constexpr Result& operator=(ErrorType&& error)		{result.error = move(error); state = ResultState::RS_ERROR; return *this;	}

	constexpr bool operator==(DataType const& value) const	{if (!ok()) return false; return result.value == value;	}
	constexpr bool operator==(ErrorType const& value) const	{if (ok()) return false; return result.error == error;	}

	constexpr bool ok()	const			{return state == ResultState::RS_OK;	}
	constexpr operator bool() const		{return ok();							}
	constexpr bool operator()() const	{return ok();							}

	constexpr Result const& operator()(Procedure<DataType const&> const& proc) const	{return then(proc);}
	constexpr Result const& operator()(Procedure<ErrorType const&> const& proc) const	{return then(proc);}

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

#endif // CTL_CONTAINER_RESULT_H
