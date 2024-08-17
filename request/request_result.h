#pragma once

#include <queue>
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
		: res_type(ResultType::OK)
	{}

	RequestResult(ResultType rt)
		: res_type(rt)
	{}

	RequestResult(ResultType rt, const std::string& et)
		: res_type(rt)
		, error_text(et)
	{}

	ResultType res_type;

	std::queue<std::string> res_strings; // ������, ���������� � ���������� ���������� �������.

	std::string error_text; // ����� ������, ���� ��� ����
};