#include <regex.h>
#include <string>
#include <vector>

class Reggy {
public:
    Reggy(int flags);

    inline static constexpr size_t NO_GROUP = (size_t)-1;

    bool setFlag(int, bool, bool recalc = true);
    bool setData(const std::string&, bool recalc = true);
    bool setPattern(const std::string&, bool recalc = true);
    bool setMultiline(bool, bool recalc = true);
    bool setPriority(size_t);

    std::string getErrorString() const;
    bool isReady() const;
    size_t getGroupCount() const;
    size_t getMatchCount() const;
    size_t getPrimaryGroup(size_t stop) const;
    size_t getStopLen(size_t stop) const;
    std::string getMatch(size_t group) const;

private:
    int flags;
    bool ok;
    bool multiline;
    regex_t re;
    std::string error;
    size_t nGroups, nMatches;
    std::string pattern;
    std::string data;
    std::vector<regmatch_t> matches;
    std::vector<size_t> stopLen;
    std::vector<size_t> primaryGroup;

    bool recompile(size_t group = NO_GROUP);
};