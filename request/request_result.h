#pragma once

#include <string>

enum class ResultType
{
	OK,
	ERR
};

// ��������� ���������� ������� � ����
struct RequestResult
{
	RequestResult()
		: result(ResultType::OK)
	{}

	ResultType result;
	std::string error; // ����� ������, ���� ��� ����
};