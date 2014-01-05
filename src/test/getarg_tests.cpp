#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-HOE");
    BOOST_CHECK(GetBoolArg("-HOE"));
    BOOST_CHECK(GetBoolArg("-HOE", false));
    BOOST_CHECK(GetBoolArg("-HOE", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-HOEo"));
    BOOST_CHECK(!GetBoolArg("-HOEo", false));
    BOOST_CHECK(GetBoolArg("-HOEo", true));

    ResetArgs("-HOE=0");
    BOOST_CHECK(!GetBoolArg("-HOE"));
    BOOST_CHECK(!GetBoolArg("-HOE", false));
    BOOST_CHECK(!GetBoolArg("-HOE", true));

    ResetArgs("-HOE=1");
    BOOST_CHECK(GetBoolArg("-HOE"));
    BOOST_CHECK(GetBoolArg("-HOE", false));
    BOOST_CHECK(GetBoolArg("-HOE", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noHOE");
    BOOST_CHECK(!GetBoolArg("-HOE"));
    BOOST_CHECK(!GetBoolArg("-HOE", false));
    BOOST_CHECK(!GetBoolArg("-HOE", true));

    ResetArgs("-noHOE=1");
    BOOST_CHECK(!GetBoolArg("-HOE"));
    BOOST_CHECK(!GetBoolArg("-HOE", false));
    BOOST_CHECK(!GetBoolArg("-HOE", true));

    ResetArgs("-HOE -noHOE");  // -HOE should win
    BOOST_CHECK(GetBoolArg("-HOE"));
    BOOST_CHECK(GetBoolArg("-HOE", false));
    BOOST_CHECK(GetBoolArg("-HOE", true));

    ResetArgs("-HOE=1 -noHOE=1");  // -HOE should win
    BOOST_CHECK(GetBoolArg("-HOE"));
    BOOST_CHECK(GetBoolArg("-HOE", false));
    BOOST_CHECK(GetBoolArg("-HOE", true));

    ResetArgs("-HOE=0 -noHOE=0");  // -HOE should win
    BOOST_CHECK(!GetBoolArg("-HOE"));
    BOOST_CHECK(!GetBoolArg("-HOE", false));
    BOOST_CHECK(!GetBoolArg("-HOE", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--HOE=1");
    BOOST_CHECK(GetBoolArg("-HOE"));
    BOOST_CHECK(GetBoolArg("-HOE", false));
    BOOST_CHECK(GetBoolArg("-HOE", true));

    ResetArgs("--noHOE=1");
    BOOST_CHECK(!GetBoolArg("-HOE"));
    BOOST_CHECK(!GetBoolArg("-HOE", false));
    BOOST_CHECK(!GetBoolArg("-HOE", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-HOE", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-HOE", "eleven"), "eleven");

    ResetArgs("-HOE -bar");
    BOOST_CHECK_EQUAL(GetArg("-HOE", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-HOE", "eleven"), "");

    ResetArgs("-HOE=");
    BOOST_CHECK_EQUAL(GetArg("-HOE", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-HOE", "eleven"), "");

    ResetArgs("-HOE=11");
    BOOST_CHECK_EQUAL(GetArg("-HOE", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-HOE", "eleven"), "11");

    ResetArgs("-HOE=eleven");
    BOOST_CHECK_EQUAL(GetArg("-HOE", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-HOE", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-HOE", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-HOE", 0), 0);

    ResetArgs("-HOE -bar");
    BOOST_CHECK_EQUAL(GetArg("-HOE", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-HOE=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-HOE", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-HOE=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-HOE", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--HOE");
    BOOST_CHECK_EQUAL(GetBoolArg("-HOE"), true);

    ResetArgs("--HOE=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-HOE", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noHOE");
    BOOST_CHECK(!GetBoolArg("-HOE"));
    BOOST_CHECK(!GetBoolArg("-HOE", true));
    BOOST_CHECK(!GetBoolArg("-HOE", false));

    ResetArgs("-noHOE=1");
    BOOST_CHECK(!GetBoolArg("-HOE"));
    BOOST_CHECK(!GetBoolArg("-HOE", true));
    BOOST_CHECK(!GetBoolArg("-HOE", false));

    ResetArgs("-noHOE=0");
    BOOST_CHECK(GetBoolArg("-HOE"));
    BOOST_CHECK(GetBoolArg("-HOE", true));
    BOOST_CHECK(GetBoolArg("-HOE", false));

    ResetArgs("-HOE --noHOE");
    BOOST_CHECK(GetBoolArg("-HOE"));

    ResetArgs("-noHOE -HOE"); // HOE always wins:
    BOOST_CHECK(GetBoolArg("-HOE"));
}

BOOST_AUTO_TEST_SUITE_END()
