#include "domain_match_pattern.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>

#include <boost/algorithm/string.hpp>

namespace adblock {

DomainMatchPattern::
DomainMatchPattern(const StringRange &domain,
                   const StringRange &pattern,
                   const bool endMatch/*= false*/)
    : Base { StringRange { domain.begin(), pattern.end() },
             false, endMatch }
{
    namespace ba = boost::algorithm;
    assert(!domain.empty());

    const auto &trimmedDomain = ba::trim_copy_if(domain, ba::is_any_of("*"));
    ba::split(m_domainTokens, trimmedDomain,
              ba::is_any_of("*"), ba::token_compress_on);

    const auto &trimmedPattern =
        ba::trim_copy_if(pattern, ba::is_any_of("*"));
    ba::split(m_patternTokens, trimmedPattern,
            ba::is_any_of("*"), ba::token_compress_on);
}

bool DomainMatchPattern::
doMatchUrl(const Uri &url) const
{
    const auto &host = url.host_range();
    if (!this->doMatch(host, m_domainTokens)) return false;

    return this->doMatch(
            UriRange { host.end(), url.end() }, m_patternTokens);
}

} // namespace adblock
