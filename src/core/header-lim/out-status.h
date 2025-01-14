#ifndef CORE_HEADER_FILE_OUT_STATUS
#define CORE_HEADER_FILE_OUT_STATUS

typedef enum{
	HF_READING_DISABLE,
	HF_DISABLED_FLAG,
	HF_HEADER_FILE_NOT_FOUND,
	HF_HEADER_FILE_IS_EMPTY,
	HF_CONTENT_READED,
}HF_OUT_STATUS;

typedef enum{
	HF_EOF_FOUND,
	HF_EMPTY_PART,
	HF_NEXT_PART,
}HF_READ_STATUS;

#endif
