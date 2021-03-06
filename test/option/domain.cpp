#include "option/domain_option.hpp"
#include "../mock_context.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/range/iterator_range.hpp>

#include <gtest/gtest.h>

namespace adblock {

namespace qi = boost::spirit::qi;

struct DomainContext : MockContext
{
    DomainContext(const Uri &uri)
        : m_uri { uri }
    {}

    const Uri &origin() const override { return m_uri; }

    Uri m_uri;
};

TEST(DomainOption, Init1)
{
    const DomainOption::Domains domains {
        "adblock.org"_r,
    };
    const DomainOption option { domains };

    EXPECT_EQ(1, option.includeDomains().size());
    EXPECT_TRUE(option.excludeDomains().empty());
}

TEST(DomainOption, Init2)
{
    const DomainOption::Domains domains {
        "~adblock.org"_r,
    };
    const DomainOption option { domains };

    EXPECT_TRUE(option.includeDomains().empty());
    EXPECT_EQ(1, option.excludeDomains().size());
}

TEST(DomainOption, Init3)
{
    const DomainOption::Domains domains {
        "adblock.org"_r,
        "~google.com"_r,
    };
    const DomainOption option { domains };

    EXPECT_EQ(1, option.includeDomains().size());
    EXPECT_EQ(1, option.excludeDomains().size());
}

TEST(DomainOption, Init4)
{
    const DomainOption::Domains domains {
        "adblock.org"_r,
        "~facebook.com"_r,
        "google.com"_r,
        "~twitter.com"_r,
    };
    const DomainOption option { domains };

    EXPECT_EQ(2, option.includeDomains().size());
    EXPECT_EQ(2, option.excludeDomains().size());
}

TEST(DomainOption, Match)
{
    const DomainOption::Domains domains {
        "adblock.org"_r,
    };
    const DomainOption option { domains };

    const auto &uri = "http://www.google.com/image.jpg"_u;
    DomainContext context { "http://www.adblock.org/"_u };

    EXPECT_TRUE(option.match(uri, context));
}

TEST(DomainOption, NoMatch)
{
    const DomainOption::Domains domains {
        "adblock.org"_r,
    };
    const DomainOption option { domains };

    const auto &uri = "http://www.google.com/image.jpg"_u;
    DomainContext context { "http://www.google.com/"_u };

    EXPECT_FALSE(option.match(uri, context));
}

TEST(DomainOption, MatchToNegative)
{
    const DomainOption::Domains domains {
        "adblock.org"_r,
        "~sub.adblock.org"_r,
    };
    const DomainOption option { domains };

    const auto &uri = "http://www.google.com/image.jpg"_u;
    DomainContext context { "http://www.sub.adblock.org/"_u };

    EXPECT_FALSE(option.match(uri, context));
}

TEST(DomainOption, OriginIsEmpty)
{
    const DomainOption::Domains domains {
        "adblock.org"_r,
        "~sub.adblock.org"_r,
    };
    const DomainOption option { domains };

    const auto &uri = "http://www.google.com/image.jpg"_u;
    DomainContext context { ""_u };

    EXPECT_FALSE(option.match(uri, context));
}

TEST(DomainOption, ExcludeDomainOnlyOption)
{
    const DomainOption::Domains domains {
        "~adblock.org"_r,
    };
    const DomainOption option { domains };

    const auto &uri = "http://www.google.com/image.jpg"_u;
    DomainContext context { "http://www.google.com"_u };

    EXPECT_TRUE(option.match(uri, context));
}

} // namespace adblock
