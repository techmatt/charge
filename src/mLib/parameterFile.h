
#ifndef CORE_UTIL_PARAMETERFILE_H_
#define CORE_UTIL_PARAMETERFILE_H_

#include <sstream>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <list>

class ParameterFile {
public:
	ParameterFile(char separator = '=', bool caseSensitive = true) {
		_Separator = separator;
		_CaseSensitive = caseSensitive;
	}

	ParameterFile(const std::string& filename, char separator = '=', bool caseSensitive = true) {
		_Separator = separator;
		_CaseSensitive = caseSensitive;
		addParameterFile(filename);
	}

	void addParameterFile(const std::string& filename) {
		std::ifstream file(filename);
        MLIB_ASSERT_STR(file.is_open(), "Failed to find file: " + filename);

		while(!file.eof()) {
			std::string line;
			getline(file, line);
			removeComments(line);
			removeSpecialCharacters(line);
			if (line.length() == 0) continue;

			size_t separator = line.find(_Separator);	//split the string at separator
			if (separator == std::string::npos)	{
				MLIB_WARNING("No seperator found in line");
				continue;
			}
			std::string attr_name = line.substr(0, separator);
			std::string attr_value = line.substr(separator + 1, line.length() - 1);
			removeSpecialCharacters(attr_name);
			removeSpecialCharacters(attr_value);
			if (attr_name.length() == 0) {
				MLIB_WARNING("Invalid attribute or value");
				continue;
			}
			if (!_CaseSensitive)	attr_name = util::toLower(attr_name);
			_Values[attr_name] = attr_value;
		}
		file.close();

		bool found = false;
		for (const auto &file : _Filenames) {
			if (file == filename)	found = true;
		}
		if (!found)	_Filenames.push_back(filename);
	}

	void reload() {
		_Values.clear();
		for(const auto &file : _Filenames)
		{
			addParameterFile(file);
		}
	}

	template<class T>
	bool readParameter(const std::string& name, T& value) const {
		if (_CaseSensitive) {
			const auto s = _Values.find(name);
			if (s == _Values.end()) {
				return false; 
			} else {
				util::convertTo(s->second, value);
				return true;
			}
		} else {
			std::string lname(name);	lname = util::toLower(lname);
			const auto s = _Values.find(name);
			if (s == _Values.end()) {
				return false; 
			} else {
				util::convertTo(s->second, value);
				return true;
			}
		}
	} 
	template<class U>
	bool readParameter(const std::string& name, std::vector<U>& value) const {
		value.clear();
		for (size_t i = 0;; i++) {
			std::stringstream ss;	ss << i;
			std::string currName = name + "[" + ss.str() + "]";
			U currValue;
			if (readParameter(currName, currValue)) {
				value.resize(i+1);
				value[i] = currValue;
			} else {
				break;
			}
		}		
		if (value.size() == 0)	return false;
		else return true;
	}

	template<class U>
	bool readParameter(const std::string& name, std::list<U>& value) const {
		value.clear();
		for (size_t i = 0;; i++) {
			std::stringstream ss;	ss << i;
			std::string currName = name + "[" + ss.str() + "]";
			U currValue;
			if (getParameterForValue(currName, currValue)) {
				value.push_back(currValue);
			} else {
				break;
			}
		}		
		if (value.size() == 0)	return false;
		else return true;
	}
	
	void print() const {
		for (auto iter = _Values.begin(); iter != _Values.end(); iter++) {
			std::cout << iter->first << " " << _Separator << " " << iter->second << std::endl;
		}
	}

    void overrideParameter(const std::string &parameter, const std::string &newValue)
    {
        _Values[parameter] = newValue;
    }

private:
	//! removes spaces and tabs at the begin and end of the string
	void removeSpecialCharacters(std::string &str) const {
		char characters[] = {' ', '\t', '\"', ';'};
		const unsigned int length = 4;
		bool found = true;
		while(str.length() && found) {
			found = false;
			for (unsigned int i = 0; i < length; i++) {
				if (*str.begin() == characters[i]) {
					str.erase(str.begin());	found = true;	break;
				}
				if (*(--str.end()) == characters[i]) {
					str.erase(--str.end()); found = true;	break;
				};
			}
		}
	}

	//! searches for comments and removes everything after the comment if there is one
	void removeComments(std::string& s) const {
		std::string comments[] = {"//", "#", ";"};
		const unsigned int length = 3;
		for (unsigned int i = 0; i < length; i++) {
			size_t curr = s.find(comments[i]);
			if (curr != std::string::npos) {
				s = s.substr(0, curr);
			}
		}
	}
	std::map<std::string, std::string> _Values;
	char _Separator;
	bool _CaseSensitive;
	std::list<std::string> _Filenames;
};

#endif  // CORE_UTIL_PARAMETERFILE_H_
