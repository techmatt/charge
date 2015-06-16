
#ifndef CORE_UTIL_PARAMETERTABLE_H_
#define CORE_UTIL_PARAMETERTABLE_H_

struct ParameterTable
{
public:
    ParameterTable()
    {
        name = "unnamed";
    }

    ParameterTable(const string &tableName)
    {
        name = tableName;
    }
    
    void setString(const string &parameterName, const string &value)
    {
        params[parameterName].push_back(value);
    }

    void setInt(const string &parameterName, int value)
    {
        params[parameterName].push_back(to_string(value));
    }

    void setBool(const string &parameterName, bool value)
    {
        setInt(parameterName, value ? 1 : 0);
    }

    template<class T>
    void set(const string &parameterName, const T &value)
    {
        params[parameterName].push_back(convert::toString(value));
    }

    template<>
    void set(const string &parameterName, const string &value)
    {
        params[parameterName].push_back(value);
    }

    template<class T>
    void get(const string &parameterName, T &result)
    {
        const string line = getString(parameterName);
        convert::to(line, result);
    }

    void setTable(const string &parameterName, const ParameterTable &table)
    {
        vector<string> &lines = params[parameterName];
        table.appendLines(lines);
    }

    bool hasParameter(const string &parameterName) const
    {
        return (params.count(parameterName) > 0);
    }

    string getString(const string &parameterName) const
    {
        MLIB_ASSERT_STR(params.count(parameterName) > 0, "Parameter not found");
        MLIB_ASSERT_STR(params.at(parameterName).size() == 1, "Multi-line parameter found");
        return params.at(parameterName).front();
    }

    int getInt(const string &parameterName) const
    {
        return convert::toInt(getString(parameterName));
    }

    bool getBool(const string &parameterName) const
    {
        return convert::toInt(getString(parameterName)) == 0 ? false : true;
    }

    vec2i getVec2i(const string &parameterName) const
    {
        vec2i result;
        convert::to(getString(parameterName), result);
        return result;
    }

    ParameterTable getTable(const string &parameterName) const
    {
        MLIB_ASSERT_STR(params.count(parameterName) > 0, "Parameter not found");
        MLIB_ASSERT_STR(params.at(parameterName).size() >= 1, "Invalid table parameter found");

        ParameterTable result;
        int lineStart = 0;
        result.fromLines(params.at(parameterName), lineStart);
        return result;
    }

    void appendLines(vector<string> &lines) const
    {
        vector<string> newLines;
        for (auto &p : params)
        {
            if (p.second.size() == 1)
                newLines.push_back(p.first + "=" + p.second.front());
            else
            {
                newLines.push_back(p.first + "~" + to_string(p.second.size()));
                for (const string &line : p.second)
                    newLines.push_back(line);
            }
        }

        lines.push_back(name + "~" + to_string(params.size()) + "~" + to_string(newLines.size()));
        for (const string &newLine : newLines)
            lines.push_back(newLine);
    }

    void fromLines(const vector<string> &lines, int &lineIndex)
    {
        const vector<string> &header = util::split(lines[lineIndex++], '~');
        name = header[0];
        
        const int paramCount = convert::toInt(header[1]);

        for (int paramIndex = 0; paramIndex < paramCount; paramIndex++)
        {
            const string &paramHeader = lines[lineIndex++];
            if (util::contains(paramHeader, '='))
            {
                auto words = util::split(paramHeader, '=');
                vector<string> &value = params[words[0]];
                value.push_back(words[1]);
            }
            else
            {
                auto words = util::split(paramHeader, '~');
                vector<string> &value = params[words[0]];
                const int paramLineCount = convert::toInt(words[1]);
                for (int paramLineIndex = 0; paramLineIndex < paramLineCount; paramLineIndex++)
                {
                    value.push_back(lines[lineIndex++]);
                }
            }
        }
    }

    void save(const string &filename)
    {
        vector<string> lines;
        appendLines(lines);
        util::saveLinesToFile(lines, filename);
    }

    void load(const string &filename)
    {
        int lineIndex = 0;
        fromLines(util::getFileLines(filename), lineIndex);
    }
    
    string name;
    map<string, vector<string> > params;
};

#endif  // CORE_UTIL_PARAMETERTABLE_H_