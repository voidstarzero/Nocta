#include <cstdlib>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include "Scanner.hpp"

using namespace std;

bool g_had_error = false;

namespace
{
    void usage(const char* argv0);

    void run_file(const char* path);
    void run_prompt();

    void run(std::string& source);

    void report(int line, int col, const char* where, const char* message);
}

int main(int argc, char** argv)
{
    if (argc > 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (argc == 2) {
        run_file(argv[1]);
    }
    else {
        run_prompt();
    }
}

void error(int line, int col, const char* message)
{
    report(line, col, "", message);
}

namespace
{
    void usage(const char* argv0)
    {
        std::cerr << "usage: " << argv0 << " [source-file]" << std::endl;
    }

    void run_file(const char* path)
    {
        std::ifstream in(path);
        std::string text(std::istreambuf_iterator<char>{in},
                         std::istreambuf_iterator<char>());

        run(text);

        if (g_had_error) exit(EXIT_FAILURE);
    }

    void run_prompt()
    {
        std::string line;
        while (cin) {
            std::cout << "> ";
            std::getline(cin, line);

            run(line);
            g_had_error = false; // Suppress to avoid aborting abnormally
        }
    }

    void run(std::string& source)
    {
        Scanner s(source);
        s.scan_tokens();

        // For now, just print the tokens
        for (const Token& token: s.get_tokens()) {
            std::cout << token.to_string() << '\n';
        }
    }

    void report(int line, int col, const char* where, const char* message)
    {
        std::cerr << "[line " << line << ", col " << col
                  << "] error"   << where << ": " << message << std::endl;
        g_had_error = true;
    }
}