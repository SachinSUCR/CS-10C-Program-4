#include "multimap.h"
#include <gtest/gtest.h>

TEST(MultimapTest, InsertAndGet) {
  Multimap<int, char> mm;
  mm.Insert(5, 'A');
  EXPECT_TRUE(mm.Contains(5));
  EXPECT_EQ(mm.Get(5), 'A');
}

TEST(MultimapTest, MultiInsert) {
  Multimap<int, char> mm;
  mm.Insert(5, 'A');
  mm.Insert(5, 'B');
  mm.Insert(5, 'C');

  EXPECT_EQ(mm.Get(5), 'A');  // always first
}

TEST(MultimapTest, RemoveSingleValue) {
  Multimap<int, char> mm;
  mm.Insert(5, 'A');
  EXPECT_TRUE(mm.Contains(5));
  mm.Remove(5);
  EXPECT_FALSE(mm.Contains(5));
}

TEST(MultimapTest, RemoveOneAtATime) {
  Multimap<int, char> mm;
  mm.Insert(5, 'A');
  mm.Insert(5, 'B');
  mm.Insert(5, 'C');

  mm.Remove(5);
  EXPECT_TRUE(mm.Contains(5));
  EXPECT_EQ(mm.Get(5), 'B');

  mm.Remove(5);
  EXPECT_EQ(mm.Get(5), 'C');

  mm.Remove(5);
  EXPECT_FALSE(mm.Contains(5));
}

TEST(MultimapTest, MinMax) {
  Multimap<int, int> mm;
  mm.Insert(20, 1);
  mm.Insert(5, 1);
  mm.Insert(11, 1);
  EXPECT_EQ(mm.Min(), 5);
  EXPECT_EQ(mm.Max(), 20);
}
