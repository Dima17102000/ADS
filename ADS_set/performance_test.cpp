

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace std;

#include "ADS_set.h"

vector<size_t> create_with_count(size_t i) {
	vector<size_t> data;
	while (i != 0) {
		data.push_back(i);
		--i;
	}
	return data;
}
void print_ns(size_t ns, ostream& os = std::cout) {
	if (ns > 9999999) {
		os << (ns / 1000 / 1000) << "ms";
	} else if (ns > 9999) {
		os << (ns / 1000) << "µs";
	} else {
		os << ns << "ns";
	}
}

int main() {
	

	size_t repeat_test = 12;  // At least 4 because of repeat_test / 4 later on
	vector<vector<size_t>> datas;
	datas.push_back(create_with_count(1));
	datas.push_back(create_with_count(10));
	datas.push_back(create_with_count(100));
	datas.push_back(create_with_count(1000));
	datas.push_back(create_with_count(10000));
	datas.push_back(create_with_count(100000));
	datas.push_back(create_with_count(1000000));
	datas.push_back(create_with_count(10000000));
	vector<ADS_set<size_t>> sets;
	for (const auto& data : datas) {
		ADS_set<size_t> set;
		for (const auto& value : data) {
			set.insert(value);
		}
		sets.push_back(set);
	}

	std::cout << "Start!\n";

	std::cout << "-------- Start: Insert Test --------\n";
	for (const auto& data : datas) {
		std::cout << "Count " << data.size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test; r++) {
			ADS_set<size_t> set;
			auto start = std::chrono::system_clock::now();
			for (const auto& value : data) {
				set.insert(value);
			}
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / repeat_test);
		std::cout << "\n";
	}
	std::cout << "-------- End: Insert Test --------\n\n";

	std::cout << "-------- Start: Range Insert Test --------\n";
	for (const auto& data : datas) {
		std::cout << "Count " << data.size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test; r++) {
			ADS_set<size_t> set;
			auto start = std::chrono::system_clock::now();
			set.insert(data.begin(), data.end());
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / repeat_test);
		std::cout << "\n";
	}
	std::cout << "-------- End: Range Insert Test --------\n\n";

	std::cout << "-------- Start: Insert and Clear Test --------\n";
	for (const auto& data : datas) {
		std::cout << "Count " << data.size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test; r++) {
			ADS_set<size_t> set;
			auto start = std::chrono::system_clock::now();
			set.insert(data.begin(), data.end());
			set.clear();
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / repeat_test);
		std::cout << "\n";
	}
	std::cout << "-------- End: Insert and Clear Test --------\n\n";

	std::cout << "-------- Start: Iterate Test --------\n";
	for (const auto& data : datas) {
		std::cout << "Count " << data.size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test; r++) {
			ADS_set<size_t> set;
			set.insert(data.begin(), data.end());
			auto start = std::chrono::system_clock::now();
			for (const auto& data : set) {
				if (data > set.size())
					std::cout << "Should not be called!\n";
			}
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / repeat_test);
		std::cout << "\n";
	}
	std::cout << "-------- End: Iterate Test --------\n\n";

	std::cout << "-------- Start: copy Test --------\n";
	for (size_t i = 0; i < datas.size(); i++) {
		std::cout << "Count " << datas[i].size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test; r++) {
			auto start = std::chrono::system_clock::now();
			ADS_set<size_t> copy{sets[i]};
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / repeat_test);
		std::cout << "\n";
	}
	std::cout << "-------- End: copy Test --------\n\n";

	std::cout << "-------- Start: operator== Test --------\n";
	for (size_t i = 0; i < datas.size(); i++) {
		std::cout << "Count " << datas[i].size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test; r++) {
			ADS_set<size_t> copy{sets[i]};
			auto start = std::chrono::system_clock::now();
			if (!(copy == sets[i])) {
				std::cerr << "Should equal at [" << i << "]\n";
			}
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / repeat_test);
		std::cout << "\n";
	}
	std::cout << "-------- End: operator== Test --------\n\n";

	std::cout << "-------- Start: find Test --------\n";
	for (size_t i = 0; i < datas.size(); i++) {
		std::cout << "Count " << datas[i].size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test / 4; r++) {
			auto start = std::chrono::system_clock::now();
			for (const auto& value : datas[i]) {
				if (sets[i].find(value) == sets[i].end()) {
					std::cerr << "Should have found that data: " << value << " at [" << i << "]\n";
				}
			}
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / (repeat_test / 4));
		std::cout << "\n";
	}
	std::cout << "-------- End: find Test --------\n\n";

	std::cout << "-------- Start: find non existing Test --------\n";
	for (size_t i = 0; i < datas.size(); i++) {
		std::cout << "Count " << datas[i].size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test; r++) {
			auto start = std::chrono::system_clock::now();
			if (sets[i].find((size_t)-1) != sets[i].end()) {
				std::cerr << "Should not have found that data: " << ((size_t)-1) << " at [" << i << "]\n";
			}
			if (sets[i].find((size_t)-420) != sets[i].end()) {
				std::cerr << "Should not have found that data: " << ((size_t)-420) << " at [" << i << "]\n";
			}
			if (sets[i].find((size_t)-9999) != sets[i].end()) {
				std::cerr << "Should not have found that data: " << ((size_t)-9999) << " at [" << i << "]\n";
			}
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / (repeat_test / 4));
		std::cout << "\n";
	}
	std::cout << "-------- End: find non existing Test --------\n\n";

	std::cout << "-------- Start: count Test --------\n";
	for (size_t i = 0; i < datas.size(); i++) {
		std::cout << "Count " << datas[i].size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test / 4; r++) {
			auto start = std::chrono::system_clock::now();
			for (const auto& value : datas[i]) {
				if (sets[i].count(value) == 0) {
					std::cerr << "Should have count that data: " << value << " at [" << i << "]\n";
				}
			}
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / (repeat_test / 4));
		std::cout << "\n";
	}
	std::cout << "-------- End: count Test --------\n\n";

	std::cout << "-------- Start: count non existing Test --------\n";
	for (size_t i = 0; i < datas.size(); i++) {
		std::cout << "Count " << datas[i].size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test; r++) {
			auto start = std::chrono::system_clock::now();
			if (sets[i].count((size_t)-1) == 1) {
				std::cerr << "Should not have count that data: " << ((size_t)-1) << " at [" << i << "]\n";
			}
			if (sets[i].find((size_t)-420) != sets[i].end()) {
				std::cerr << "Should not have count that data: " << ((size_t)-420) << " at [" << i << "]\n";
			}
			if (sets[i].find((size_t)-9999) != sets[i].end()) {
				std::cerr << "Should not have count that data: " << ((size_t)-9999) << " at [" << i << "]\n";
			}
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / repeat_test);
		std::cout << "\n";
	}
	std::cout << "-------- End: count non existing Test --------\n\n";

	std::cout << "-------- Start: erase Test --------\n";
	for (size_t i = 0; i < datas.size(); i++) {
		std::cout << "Count " << datas[i].size() << ":\t\t";
		size_t time = 0;
		for (size_t r = 0; r < repeat_test; r++) {
			auto start = std::chrono::system_clock::now();
			for (const auto& value : datas[i]) {
				sets[i].erase(value);
			}
			if(!sets[i].empty()) {
				std::cerr << "Set [" << i << "] should be empty!\n";
			}
			auto end = std::chrono::system_clock::now();
			time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}
		print_ns(time / repeat_test);
		std::cout << "\n";
	}
	std::cout << "-------- End: erase Test --------\n\n";

	std::cout << "End!\n";
}
