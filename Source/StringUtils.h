#pragma once

void CoutVector(const std::vector<std::string>& strings)
{
	std::cout << "[";

	if (strings.size() == 0)
	{
		std::cout << "]";
	}
	else
	{
		std::for_each(strings.begin(), strings.end() - 1, [](const std::string& element) {
			std::cout << element << ", ";
		});
		std::cout << strings.back() << "]";
	}
}

void CoutVector(const std::vector<unsigned int>& values)
{
	std::vector<std::string> strings;
	std::transform(values.begin(), values.end(), std::back_inserter(strings), [](const int& value) { return std::to_string(value); });

	CoutVector(strings);
}

void SplitStringUsingRegex(const std::string &str, std::vector<std::string>& tokens, const std::regex expression)
{
	std::sregex_token_iterator tokensBegin(str.begin(), str.end(), expression, -1);
	std::sregex_token_iterator tokensEnd;
	tokens.assign(tokensBegin, tokensEnd);
}