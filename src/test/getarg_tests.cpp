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
    ResetArgs("-noise");
    BOOST_CHECK(GetBoolArg("-noise"));
    BOOST_CHECK(GetBoolArg("-noise", false));
    BOOST_CHECK(GetBoolArg("-noise", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-noiseo"));
    BOOST_CHECK(!GetBoolArg("-noiseo", false));
    BOOST_CHECK(GetBoolArg("-noiseo", true));

    ResetArgs("-noise=0");
    BOOST_CHECK(!GetBoolArg("-noise"));
    BOOST_CHECK(!GetBoolArg("-noise", false));
    BOOST_CHECK(!GetBoolArg("-noise", true));

    ResetArgs("-noise=1");
    BOOST_CHECK(GetBoolArg("-noise"));
    BOOST_CHECK(GetBoolArg("-noise", false));
    BOOST_CHECK(GetBoolArg("-noise", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nonoise");
    BOOST_CHECK(!GetBoolArg("-noise"));
    BOOST_CHECK(!GetBoolArg("-noise", false));
    BOOST_CHECK(!GetBoolArg("-noise", true));

    ResetArgs("-nonoise=1");
    BOOST_CHECK(!GetBoolArg("-noise"));
    BOOST_CHECK(!GetBoolArg("-noise", false));
    BOOST_CHECK(!GetBoolArg("-noise", true));

    ResetArgs("-noise -nonoise");  // -noise should win
    BOOST_CHECK(GetBoolArg("-noise"));
    BOOST_CHECK(GetBoolArg("-noise", false));
    BOOST_CHECK(GetBoolArg("-noise", true));

    ResetArgs("-noise=1 -nonoise=1");  // -noise should win
    BOOST_CHECK(GetBoolArg("-noise"));
    BOOST_CHECK(GetBoolArg("-noise", false));
    BOOST_CHECK(GetBoolArg("-noise", true));

    ResetArgs("-noise=0 -nonoise=0");  // -noise should win
    BOOST_CHECK(!GetBoolArg("-noise"));
    BOOST_CHECK(!GetBoolArg("-noise", false));
    BOOST_CHECK(!GetBoolArg("-noise", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--noise=1");
    BOOST_CHECK(GetBoolArg("-noise"));
    BOOST_CHECK(GetBoolArg("-noise", false));
    BOOST_CHECK(GetBoolArg("-noise", true));

    ResetArgs("--nonoise=1");
    BOOST_CHECK(!GetBoolArg("-noise"));
    BOOST_CHECK(!GetBoolArg("-noise", false));
    BOOST_CHECK(!GetBoolArg("-noise", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-noise", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-noise", "eleven"), "eleven");

    ResetArgs("-noise -bar");
    BOOST_CHECK_EQUAL(GetArg("-noise", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-noise", "eleven"), "");

    ResetArgs("-noise=");
    BOOST_CHECK_EQUAL(GetArg("-noise", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-noise", "eleven"), "");

    ResetArgs("-noise=11");
    BOOST_CHECK_EQUAL(GetArg("-noise", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-noise", "eleven"), "11");

    ResetArgs("-noise=eleven");
    BOOST_CHECK_EQUAL(GetArg("-noise", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-noise", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-noise", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-noise", 0), 0);

    ResetArgs("-noise -bar");
    BOOST_CHECK_EQUAL(GetArg("-noise", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-noise=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-noise", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-noise=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-noise", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--noise");
    BOOST_CHECK_EQUAL(GetBoolArg("-noise"), true);

    ResetArgs("--noise=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-noise", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nonoise");
    BOOST_CHECK(!GetBoolArg("-noise"));
    BOOST_CHECK(!GetBoolArg("-noise", true));
    BOOST_CHECK(!GetBoolArg("-noise", false));

    ResetArgs("-nonoise=1");
    BOOST_CHECK(!GetBoolArg("-noise"));
    BOOST_CHECK(!GetBoolArg("-noise", true));
    BOOST_CHECK(!GetBoolArg("-noise", false));

    ResetArgs("-nonoise=0");
    BOOST_CHECK(GetBoolArg("-noise"));
    BOOST_CHECK(GetBoolArg("-noise", true));
    BOOST_CHECK(GetBoolArg("-noise", false));

    ResetArgs("-noise --nonoise");
    BOOST_CHECK(GetBoolArg("-noise"));

    ResetArgs("-nonoise -noise"); // noise always wins:
    BOOST_CHECK(GetBoolArg("-noise"));
}

BOOST_AUTO_TEST_SUITE_END()
