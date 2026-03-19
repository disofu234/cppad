#include <vector>
#include <list>
#include <chrono>
#include <random>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "chars.h"

enum class OP { INSERT };

struct COMMAND
{
	OP op;
	char ch;
};

using LINE_COMMANDS = std::vector<COMMAND>;
using DOCUMENT_COMMANDS = std::vector<LINE_COMMANDS>;

DOCUMENT_COMMANDS generate_commands(int total_chars, int avg_chars_per_line, std::mt19937& rng)
{
	std::exponential_distribution<double> line_len_dist(1.0 / avg_chars_per_line);
	std::uniform_int_distribution<int> char_dist('a', 'z');

	DOCUMENT_COMMANDS doc;
	int remaining = total_chars;

	while (remaining > 0)
	{
		int line_len = std::max(1, (int)std::lround(line_len_dist(rng)));
		line_len = std::min(line_len, remaining);

		LINE_COMMANDS line;
		line.reserve(line_len);
		for (int i = 0; i < line_len; i++)
			line.push_back({OP::INSERT, static_cast<char>(char_dist(rng))});

		doc.push_back(std::move(line));
		remaining -= line_len;
		if (remaining > 0)
			remaining--;  // newline between lines
	}

	return doc;
}

double bench_piece_table(const DOCUMENT_COMMANDS& doc)
{
	auto start = std::chrono::high_resolution_clock::now();

	std::list<CHARS> content;
	content.push_back(CHARS());
	auto shared_buf = content.front().get_buffer();

	for (size_t i = 0; i < doc.size(); i++)
	{
		CHARS& line = content.back();
		for (const COMMAND& cmd : doc[i])
		{
			if (cmd.op == OP::INSERT)
				line.insert(line.end(), cmd.ch);
		}
		if (i + 1 < doc.size())
			content.push_back(CHARS(shared_buf));
	}

	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double, std::milli>(end - start).count();
}

double bench_linked_list(const DOCUMENT_COMMANDS& doc)
{
	auto start = std::chrono::high_resolution_clock::now();

	std::list<std::list<char>> content;
	content.push_back({});

	for (size_t i = 0; i < doc.size(); i++)
	{
		std::list<char>& line = content.back();
		for (const COMMAND& cmd : doc[i])
		{
			if (cmd.op == OP::INSERT)
				line.push_back(cmd.ch);
		}
		if (i + 1 < doc.size())
			content.push_back({});
	}

	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double, std::milli>(end - start).count();
}

int main()
{
	const std::vector<int> total_chars_list  = {100, 1000, 10000, 100000};
	const std::vector<int> avg_line_len_list = {10, 100, 1000, 10000};
	const int runs = 3;

	std::ofstream out("bench_results.csv");
	out << "total_chars,avg_chars_per_line,run,piece_table_ms,linked_list_ms,speedup\n";

	std::mt19937 rng(42);

	for (int total_chars : total_chars_list)
	{
		for (int avg_line_len : avg_line_len_list)
		{
			double pt_total = 0, ll_total = 0;

			for (int run = 1; run <= runs; run++)
			{
				DOCUMENT_COMMANDS doc = generate_commands(total_chars, avg_line_len, rng);

				double pt_ms = bench_piece_table(doc);
				double ll_ms = bench_linked_list(doc);

				pt_total += pt_ms;
				ll_total += ll_ms;

				double speedup = ll_ms / pt_ms;
				out << total_chars << "," << avg_line_len << "," << run << ","
				    << pt_ms << "," << ll_ms << "," << speedup << "\n";

				std::cout << "total=" << total_chars
				          << " avg_line=" << avg_line_len
				          << " run=" << run
				          << "  piece_table=" << pt_ms << "ms"
				          << "  linked_list=" << ll_ms << "ms"
				          << "  speedup=" << speedup << "x\n";
			}

			double pt_avg = pt_total / runs;
			double ll_avg = ll_total / runs;

			out << total_chars << "," << avg_line_len << ",avg,"
			    << pt_avg << "," << ll_avg << "," << ll_avg / pt_avg << "\n";

			std::cout << "=> avg  piece_table=" << pt_avg << "ms"
			          << "  linked_list=" << ll_avg << "ms"
			          << "  speedup=" << ll_avg / pt_avg << "x\n\n";
		}
	}

	out.close();
	std::cout << "Results saved to bench_results.csv\n";
	return 0;
}
