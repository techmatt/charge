
#include "main.h"

vector< map< string, string > > GameUtil::readCSVFile(const string &filename)
{
    const auto lines = util::getFileLines(filename);

    const auto header = util::split(lines[0], ',');

    vector< map< string, string > > result;

    for (int lineIndex = 1; lineIndex < lines.size(); lineIndex++)
    {
        result.push_back(map< string, string >());
        auto &line = result.back();

        int wordIndex = 0;
        for (const string &word : util::split(lines[lineIndex], ','))
        {
            if (wordIndex < header.size())
                line[header[wordIndex++]] = word;
        }
    }

    //
    // TODO: deal with reading csv's with commas in description
    //

    return result;
}