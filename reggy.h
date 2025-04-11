#include <regex.h>
#include <string>
#include <vector>
#include <algorithm>

class Reggy {
    int flags;
    bool ok;
    bool multiline;
    bool exclusive;
    regex_t re;
    size_t priority;
    size_t nGroups, nMatches;
    std::string error;
    std::string pattern;
    std::string data;
    std::vector<size_t> stopLen;
    std::vector<size_t> primaryGroup;

    bool recompile();

public:
    inline static constexpr size_t NO_GROUP = (size_t)-1;

    Reggy(int flags);

    bool setFlag(int flag, bool value, bool recalc = true);
    bool setFlags(int flags, bool recalc = true);
    bool setData(const std::string& data, bool recalc = true);
    bool setPattern(const std::string& data, bool recalc = true);
    bool setMultiline(bool value, bool recalc = true);
    bool setPriority(size_t group, bool exclusive = false, bool recalc = true);

    std::string getErrorString() const;
    bool isReady() const;
    int getFlags() const;
    size_t getPriority() const;
    size_t getGroupCount() const;
    size_t getMatchCount() const;
    size_t getPrimaryGroup(size_t stop) const;
    size_t getStopLen(size_t stop) const;
};