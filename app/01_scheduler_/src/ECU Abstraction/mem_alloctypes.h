/****************************************
*
*	File: mem_alloctypes.h
*	Contains all the internal data types definitions use by the memory allocation handler Module
*
****************************************/

typedef void* MemReturnType;
typedef uint16_t MemSizeType;

typedef struct{
	uint8_t* MemStart;
	uint8_t* MemEnd;
	uint8_t* CurrAddr;
	uint32_t* FreeBytes;
} MemHandlerType;