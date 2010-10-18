// this file describes basic usage of sink_stdio/sink_string/sink_ostream
#include "ntfmt_stdio.hpp"
#include "ntfmt_string.hpp"
#include "ntfmt_ostream.hpp"

#include <iostream>
#include <sstream>

int main() {
	{
		// pass FILE * to sink_cfile
		ntfmt::sink_cfile(stdout) << "dk3ljfla\n";
	}
	{
		std::string s;
		// pass std::(w)string & to (w)sink_string.
		ntfmt::sink_string(s) << 3248190 << '\n';
		std::cout << s;
		std::wstring ws;
		ntfmt::sink_wstring(ws) << 932. << L'\n';
		std::wcout << ws;
	}
	{
		std::stringstream ss;
		// pass std::(w)ostream & to sink_(w)ostream
		ntfmt::sink_ostream(ss) << 3848.231 << '\n';
		std::cout << ss.str();
	}
}
