namespace Language {
	struct TypeInfo {
		size_t	id;
		size_t	size;
	};

	typedef long long Index;

	typedef Index TypeIndex;

	struct DataBlock {
		void*	data = nullptr;
		Index	type = -2;
	};

	#define TYPEINFO(TYPE, ID) TypeInfo{(ID), sizeof(TYPE)}
	#define SU_TYPEINFO(TYPE, ID) TYPEINFO(sizeof(TYPE), (ID)), TYPEINFO(sizeof(unsigned TYPE), (ID)+1)
	TypeInfo const systemTypes[] = {
		TYPEINFO(		decltype(nullptr),	-1	),
		TypeInfo{0, 0}, // void
		TYPEINFO(		void*,				1	),
		SU_TYPEINFO(	char,				2	),
		SU_TYPEINFO(	short,				4	),
		SU_TYPEINFO(	int,				6	),
		SU_TYPEINFO(	long,				6	),
		SU_TYPEINFO(	long long,			8	),
		TYPEINFO(		float,				10	),
		TYPEINFO(		double,				11	),
		TYPEINFO(		long double,		12	)
	};
	#undef SU_TYPEINFO
	#undef TYPEINFO

	template <typename T> constexpr TypeIndex		getSystemTypeIndex();
	template <typename T> constexpr TypeInfo const&	getSystemTypeInfo();

	#define SYS_TYPE_FUNC(TYPE, INDEX)\
		template<> TypeIndex		getSystemTypeIndex<TYPE>()	{return (INDEX);				}\
		template<> TypeInfo const&	getSystemTypeInfo<TYPE>()	{return systemTypes[(INDEX+1)];	}\

	#define SU_SYS_TYPE_FUNC(TYPE, INDEX)\
		SYS_TYPE_FUNC(TYPE, (INDEX))\
		SYS_TYPE_FUNC(unsigned TYPE, (INDEX)+1)

	SYS_TYPE_FUNC(		decltype(nullptr),	-1	)
	SYS_TYPE_FUNC(		void,				0	)
	SYS_TYPE_FUNC(		void*,				1	)

	SU_SYS_TYPE_FUNC(	char,				2	)
	SU_SYS_TYPE_FUNC(	short,				4	)
	SU_SYS_TYPE_FUNC(	int,				6	)
	SU_SYS_TYPE_FUNC(	long,				6	)
	SU_SYS_TYPE_FUNC(	long long,			8	)

	SYS_TYPE_FUNC(		float,				10	)
	SYS_TYPE_FUNC(		double,				11	)
	SYS_TYPE_FUNC(		long double,		12	)

	#undef SU_SYS_TYPE_FUNC
	#undef SYS_TYPE_FUNC

	enum class ConstantType: uint8 {
		CT_NULL,
		CT_VOIDPTR,
		CT_VOID,
		CT_BYTE,
		CT_UBYTE,
		CT_CHAR		= CT_BYTE,
		CT_UCHAR	= CT_UBYTE,
		CT_SHORT,
		CT_USHORT,
		CT_INT,
		CT_UINT,
		CT_LONG		= CT_INT,
		CT_ULONG	= CT_UINT,
		CT_LLONG,
		CT_ULLONG,
		CT_FLOAT,
		CT_DOUBLE,
		CT_LDOUBLE,
	};

	enum class InstructionTarget: uint16 {
		IT_DISCARD,
		IT_CONSTANT,
		IT_REGISTER,
		IT_STACK,
	};

	enum class InstructionMethod: uint64 {
		IM_ADD,
		IM_SUB,
		IM_MUL,
		IM_DIV,
		IM_POW,
		IM_MOD,
		IM_PUSH,
		IM_POP,
		IM_STORE,
		IM_LOAD,
		IM_MOVE,
		IM_SWAP,
		IM_COPY,
		IM_CALL,
		IM_SIN,
		IM_COS,
		IM_TAN,
		IM_SQRT,
		IM_ROOT,
		IM_JUMP,
		IM_CMP_EQ,
		IM_CMP_LT,
		IM_CMP_GT,
		IM_NOT,
		IM_AND,
		IM_OR,
		IM_XOR,
		IM_JUMP_IF,
		IM_JUMP_IF_NOT,
	};

	typedef uint64 InstructionValue;

	template<size_t ARGC>
	struct Instruction {
		InstructionMethod	method;
		InstructionTarget	targets[ARGC];
		InstructionValue	values[ARGC];
	};

	enum class CodeType {
		ICT_ASSEMBLY
	};

	typedef void*(*APICallable)(void**);

	struct APIFunction {
		APICallable		func	= nullptr;
		List<TypeIndex>	argt	= {};
		TypeIndex		argr	= getSystemTypeIndex<void>();
	};

	struct Interpreter {
		enum class FailureOperation {
			IFO_REPLACE,
			IFO_IGNORE,
			IFO_CRASH_AND_BURN
		};

		Interpreter() {}

		Interpreter(List<byte> bytecode) {this->bytecode = bytecode;}

		Interpreter(String const& code, CodeType const& type) {compile(code, type);}

		constexpr Interpreter& compile(String const& code, CodeType const& type) {
			return *this;
		}

		constexpr Interpreter& loadBytecode(List<byte> const& bytecode) {
			this->bytecode = bytecode;
			return *this;
		}

		Interpreter& run() {
			if (isRunning) return *this;
			isRunning = true;
			frame = 0;
			return *this;
		}

		Interpreter& stop() {
			if (!isRunning) return *this;
			isRunning = false;
			frame = 0;
			clearUserTypes();
			return *this;
		}

		Interpreter& yield() {
			if (!isRunning) return *this;
			return *this;
		}

		Interpreter& addAPIFunction(
			String const&			name,
			APIFunction const&		func,
			FailureOperation const&	onDuplicate = FailureOperation::IFO_IGNORE
		) {
			if (apiFuncs.contains(name))
				switch(onDuplicate) {
					default:
					case FailureOperation::IFO_IGNORE: return *this;
					case FailureOperation::IFO_REPLACE: apiFuncs[name] = func; return *this;
					case FailureOperation::IFO_CRASH_AND_BURN:
						throw Error::FailedAction(
							"Function '"+name+"' already exists!",
							__FILE__,
							toString(__LINE__),
							"Interpreter::addAPIFunction"
						);
				}
			apiFuncs[name] = func;
		}

		Interpreter& removeAPIFunction(String const& name) {
			apiFuncs.erase(name);
			return *this;
		}

		Interpreter& clearAPIFunctions() {
			apiFuncs.clear();
			return *this;
		}

		constexpr bool running() {return isRunning;}

		TypeIndex addAPIType(TypeInfo const& type) {
			apiTypes.push_back(type);
			return apiTypes.size()-1;
		}

		Interpreter& removeAPIType(TypeIndex const& type) {
			apiTypes.erase(apiTypes.begin()+1);
			return *this;
		}

		Interpreter& clearAPITypes() {
			apiTypes.clear();
			return *this;
		}

	private:
		bool isRunning	= false;
		size_t frame	= 0;

		constexpr TypeIndex addUserType(TypeInfo const& type) {
			userTypes.push_back(type);
			return userTypes.size()-1;
		}

		constexpr Interpreter& clearUserTypes() {
			userTypes.clear();
			return *this;
		}

		List<DataBlock>	data;
		DataBlock		reg[64];

		List<byte> bytecode;

		List<TypeInfo> apiTypes;
		List<TypeInfo> userTypes;

		Dictionary<APIFunction>	apiFuncs;
	};

}
