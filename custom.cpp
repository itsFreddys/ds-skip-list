#include "SkipList.hpp"
#include "catch_amalgamated.hpp"

namespace {
     TEST_CASE("xEmpty", "[skip-list-public]")
	{
		SkipList<unsigned, unsigned> sl;
		REQUIRE( 2 == sl.numLayers());
		REQUIRE( 0 == sl.size() );
		REQUIRE( sl.isEmpty() );
	}

	TEST_CASE("xSimpleInteger", "[skip-list-public]")
	{
		SkipList<unsigned, unsigned> sl;
		sl.insert(3, 5);
		REQUIRE( sl.find(3) == 5 );
	}

	TEST_CASE("xSimpleString", "[skip-list-public]")
	{
		SkipList<std::string, std::string> sl;
		sl.insert("Shindler", "ICS 46");
		REQUIRE(sl.find("Shindler") == "ICS 46");
	}

    TEST_CASE("xSimpleHeightsTest", "[skip-list-public]")
	{
		SkipList<unsigned, unsigned> sl;
		std::vector<unsigned> heights;
		for(unsigned i=0; i < 10; i++)
		{
			sl.insert(i,i);
			heights.push_back( sl.height(i) );
		}
		std::vector<unsigned> expectedHeights = {1, 2, 1, 3, 1, 2, 1, 4, 1, 2};
		REQUIRE( heights == expectedHeights );
	}

	TEST_CASE("xInvolvedHeightsTest", "[skip-list-public]")
	{
		SkipList<unsigned, unsigned> sl;
		std::vector<unsigned> heights;
		for (unsigned i = 0; i < 10; i++)
		{
			sl.insert(i, i);
			heights.push_back(sl.height(i));
		}

		// The coinFlip function will always return heads
		// for 255 regardless of the current layer.
		// You can use this value to test your threshold for halting
		// the insertion procedure. If this results in an infinite loop inside
		// of your insert function you have not implemented a cutoff threshold.
		unsigned const MAGIC_VAL = 255;
		sl.insert(MAGIC_VAL, MAGIC_VAL);

		heights.push_back(sl.height(MAGIC_VAL));

		// The expected height for 255 is 12 because there are fewer than 16 nodes in
		// the skip list when 12 is added.
		std::vector<unsigned> expectedHeights = {1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 12};
		REQUIRE(heights == expectedHeights);

		// At this point there should be 13 layers
		// (because the fast lane is not included in the height calculation).
		REQUIRE(sl.numLayers() == 13);
	}

	TEST_CASE("xCapacity17Test", "[skip-list-public]")
	{
		SkipList<unsigned, unsigned> sl;
		std::vector<unsigned> heights;

		// First insert 16 values into the skip list [0, 15].
		for (unsigned i = 0; i < 16; i++)
		{
			sl.insert(i, i);
			heights.push_back(sl.height(i));
		}
		
		// Same value used in SimpleHeightsTest for testing the threshold.
		unsigned const MAGIC_VAL = 255;
		sl.insert(MAGIC_VAL, MAGIC_VAL);

		heights.push_back(sl.height(MAGIC_VAL));

		// The expected height for 255 is 15 because 3 * ceil(log_2(17)) = 15
		// meaning the maximum height of any node should be 15 for a skip list with 17 nodes.
		std::vector<unsigned> expectedHeights = {1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 15};
		REQUIRE(heights == expectedHeights);

		// At this point there should be 16 layers
		// (because the fast lane is not included in the height calculation).
		REQUIRE(sl.numLayers() == 16);
	}
}