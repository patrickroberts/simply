#include <simply/destructible.hpp>
#include <simply/dyn.hpp>
#include <simply/insertable.hpp>

#include <gtest/gtest.h>

#include <numbers>

TEST(Stream, TypeErasedInsertion) {
  using namespace std::string_literals;

  struct affordances : simply::conjunction<simply::insertable<std::ostream>,
                                           simply::destructible> {};

  std::vector<simply::dyn<affordances>> values;
  values.emplace_back("Hello, world!"s);
  values.emplace_back(4);
  values.emplace_back(std::numbers::pi);

  std::ostringstream output;

  for (const auto &value : values) {
    output << value << '\n';
  }

  EXPECT_EQ("Hello, world!\n"
            "4\n"
            "3.14159\n",
            output.str());
}
