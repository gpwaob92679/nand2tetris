#include "addressing.h"

#include "absl/strings/str_format.h"
#include "gtest/gtest.h"

TEST(DestinationTest, DestinationString) {
  EXPECT_EQ(DestinationString(Destination::kA), "A");
  EXPECT_EQ(DestinationString(Destination::kD), "D");
  EXPECT_EQ(DestinationString(Destination::kM), "M");
  EXPECT_EQ(DestinationString(Destination::kA | Destination::kD), "AD");
  EXPECT_EQ(DestinationString(Destination::kA | Destination::kM), "AM");
  EXPECT_EQ(DestinationString(Destination::kD | Destination::kM), "DM");
  EXPECT_EQ(
      DestinationString(Destination::kA | Destination::kD | Destination::kM),
      "ADM");
}

TEST(AddressingTest, ArgumentAddress) {
  EXPECT_EQ(ArgumentAddress(2).AddressingAssembly(Destination::kA),
            "@2\n"
            "D=A\n"
            "@ARG\n"
            "A=D+M\n");
}

TEST(AddressingTest, LocalAddress) {
  EXPECT_EQ(LocalAddress(2).AddressingAssembly(Destination::kA),
            "@2\n"
            "D=A\n"
            "@LCL\n"
            "A=D+M\n");
}

TEST(AddressingTest, ThisAddress) {
  EXPECT_EQ(ThisAddress(2).AddressingAssembly(Destination::kA),
            "@2\n"
            "D=A\n"
            "@THIS\n"
            "A=D+M\n");
}

TEST(AddressingTest, ThatAddress) {
  EXPECT_EQ(ThatAddress(2).AddressingAssembly(Destination::kA),
            "@2\n"
            "D=A\n"
            "@THAT\n"
            "A=D+M\n");
}

TEST(AddressingTest, StaticAddress) {
  EXPECT_EQ(StaticAddress("Foo", 2).AddressingAssembly(Destination::kA),
            "@Foo.2\n"
            "A=M\n");
}

TEST(AddressingTest, ConstantAddress) {
  EXPECT_EQ(ConstantAddress(2).AddressingAssembly(Destination::kA), "@2\n");
  EXPECT_EQ(ConstantAddress(2).AddressingAssembly(Destination::kD),
            "@2\n"
            "D=A\n");
}

TEST(AddressingTest, PointerAddress) {
  for (int i = 0; i <= 2; ++i) {
    EXPECT_EQ(PointerAddress(i).AddressingAssembly(Destination::kA),
              absl::StrFormat("@%d\n", 3 + i));
    EXPECT_EQ(PointerAddress(i).AddressingAssembly(Destination::kD),
              absl::StrFormat("@%d\n"
                              "D=A\n",
                              3 + i));
  }
}

TEST(AddressingTest, TempAddress) {
  for (int i = 5; i <= 12; ++i) {
    EXPECT_EQ(TempAddress(i).AddressingAssembly(Destination::kA),
              absl::StrFormat("@%d\n", 5 + i));
    EXPECT_EQ(TempAddress(i).AddressingAssembly(Destination::kD),
              absl::StrFormat("@%d\n"
                              "D=A\n",
                              5 + i));
  }
}
