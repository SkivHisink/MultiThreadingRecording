#pragma once
#include <chrono>

struct Time_struct
{
	std::chrono::time_point<std::chrono::steady_clock> begin;
	std::chrono::time_point<std::chrono::steady_clock> end;
	std::chrono::time_point<std::chrono::steady_clock> paused_begin;
	std::chrono::time_point<std::chrono::steady_clock> paused_end;
	std::chrono::duration<long long, std::ratio<1, 1000>> paused_summ;
	std::chrono::duration<long long, std::ratio<1, 1000>> elapsed_ms;
};
