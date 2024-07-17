#pragma once

#include <string>

enum class ResultType
{
	OK,
	ERR
};

// Результат выполнения запроса к базе
struct RequestResult
{
	RequestResult()
		: result(ResultType::OK)
	{}

	ResultType result;
	std::string error; // Текст ошибки, если она была
};