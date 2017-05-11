/**
 * Digitally signs scripts.
 *  # sign foo.py
 *  # sign --iszip foo.zip
 */

#include "signatures.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, const char** argv)
{
    std::vector<const char*> args;
    bool iszip = false;
    bool help = false;
    bool error = false;

    for (int i = 1; i < argc; ++i)
    {
        const char* arg = argv[i];
        if (arg[0] == '-')
        {
            if (strcmp(arg, "--iszip") == 0)
                iszip = true;
            else if (strcmp(arg, "--help") == 0)
                help = true;
            else
                error = true;
        }
        else
        {
            args.push_back(arg);
        }
    }
    if (args.size() != 1)
        error = true;

    if (error || help)
    {
        std::cerr << "usage: sign [--iszip] INFILE\n";
        return (int)error;
    }
    const char* infile = args[0];

    try
    {
        if (iszip)
        {
            std::string outfile = infile;
            outfile += ".standalone";
            makeStandalone(infile, outfile.c_str());
        }
        else
            createDetachedSignature(infile);
        return 0;
    }
    catch (std::exception& exc)
    {
        std::cerr << exc.what() << "\n";
        return 1;
    }
}
