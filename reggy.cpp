#include "reggy.h"

Reggy::Reggy(int _flags) : flags(_flags), multiline(false), ok(false), priority(NO_GROUP) {}

bool Reggy::setFlag(int flag, bool value, bool recalc)
{
    flags = value ? (flags | flag) : (flags & ~flag);
    return recalc ? recompile() : true;
}

bool Reggy::setFlags(int f, bool recalc)
{
    flags = f;
    return recalc ? recompile() : true;
}

bool Reggy::setMultiline(bool value, bool recalc) {
    multiline = value;
    return recalc ? recompile() : true;
}

bool Reggy::setData(const std::string& d, bool recalc)
{
    data = d;
    return recalc ? recompile() : true;
}

bool Reggy::setPattern(const std::string& p, bool recalc)
{
    pattern = p;
    return recalc ? recompile() : true;
}

bool Reggy::setPriority(size_t p, bool recalc)
{
    if (p != NO_GROUP && (!ok || p >= nGroups)) return false;
    priority = p;
    return recalc ? recompile() : true;
}

bool Reggy::isReady() const
{
    return ok;
}

int Reggy::getFlags() const
{
    return flags;
}

size_t Reggy::getPriority() const
{
    return priority;
}


size_t Reggy::getGroupCount() const
{
    return ok ? nGroups : 0;
}

size_t Reggy::getMatchCount() const
{
    return ok ? nMatches : 0;
}

std::string Reggy::getMatch(size_t group) const
{
    if (ok && group == NO_GROUP) return data;
    if (group >= matches.size() || matches[group].rm_so < 0) return "";
    return data.substr(matches[group].rm_so, matches[group].rm_eo - matches[group].rm_so);
}

size_t Reggy::getPrimaryGroup(size_t stop) const
{
    return primaryGroup.at(stop);
}

size_t Reggy::getStopLen(size_t stop) const
{
    return stopLen.at(stop);
}

std::string Reggy::getErrorString() const
{
    return error;
}

bool Reggy::recompile()
{
    ok = false;
    stopLen.clear();
    primaryGroup.clear();
    nMatches = 0;
    nGroups = std::count(pattern.begin(), pattern.end(), '(') + 1;
    regmatch_t matches[nGroups];
    if (priority >= nGroups) {
        priority = NO_GROUP;
    }

    if (data.empty()) {
        error = "";
        return false;
    }

    int result = regcomp(&re, pattern.c_str(), flags);
    if (result) {
        size_t n = regerror(result, &re, NULL, 0);
        if (n != 0) {
            char buffer[n];
            (void)regerror(result, &re, buffer, sizeof(buffer));
            error = "Error: " + std::string(buffer);
        }
        return false;
    }

    for (size_t step, offset = 0; offset < data.size(); offset += step) {
        auto n = data.find('\n', offset);
        if (n == std::string::npos) n = data.size();
        step = multiline ? data.size() : n - offset;
        size_t group = NO_GROUP;
        size_t pos = 0;

        if (step > 0 && !regexec(&re, data.substr(offset, step).c_str(), nGroups, matches, 0)) {
            for (auto& m : matches) {
                if (m.rm_so >= 0) {
                    ++nMatches;
                }
            }

            // simple version
            for (size_t i = 0; i < step; ++i) {
                size_t next = NO_GROUP;
                for (size_t k = 0; k < nGroups; ++k) {
                    if (i >= matches[k].rm_so && i < matches[k].rm_eo) {
                        next = k;
                        if (k == priority) break;
                    }
                }
                if (next != group) {
                    if (i - pos > 0) {
                        stopLen.push_back(i - pos);
                        primaryGroup.push_back(group);
                    }
                    group = next;
                    pos = i;
                }
            }
        }

        stopLen.push_back(++step - pos);
        primaryGroup.push_back(group);
    }

    if (nMatches == 0) {
        error = "No match.";
        return false;
    }

    ok = true;
    return true;
}