//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "gmock.vptr.h"
#include <stdexcept>
#include "gtest/gtest.h"

class interface {
public:
  virtual bool get() const = 0;
  virtual ~interface() = default;
};

class extended_interface : public interface {
public:
  virtual void foo(bool) = 0;
  virtual void bar(bool) = 0;
};

class example {
public:
  example(const interface& i, extended_interface& ei)
    : i(i), ei(ei)
  { }

  void update() {
    const auto value = i.get();
    if (value) {
      ei.foo(value);
    } else {
      ei.bar(value);
    }
  }

private:
  const interface& i;
  extended_interface& ei;
};

TEST(GMockVptr, ShouldMockSimpleInterface) {
  using namespace testing;
  GMock<interface> mock;
  EXPECT_CALL(mock, (get)()).WillOnce(Return(true));

  interface& i = mock;
  EXPECT_TRUE(i.get());
}

TEST(GMockVptr, ShouldMockExtendedInterface) {
  using namespace testing;
  GMock<interface> imock;
  GMock<extended_interface> emock;

  EXPECT_CALL(imock, (get)()).WillOnce(Return(true));
  EXPECT_CALL(emock, (foo)(true)).Times(1);

  example e{imock, emock};
  e.update();
}

TEST(GMockVptr, ShouldMockExtendedInterfaceDifferentMockTypes) {
  using namespace testing;
  NiceGMock<interface> imock;
  StrictGMock<extended_interface> emock;

  EXPECT_CALL(imock, (get)()).WillOnce(Return(false));
  EXPECT_CALL(emock, (bar)(false)).Times(1);

  example e{imock, emock};
  e.update();
}

TEST(GMockVptr, ShouldWorkWithMacroDefinedMocks) {
  using namespace testing;
  NiceGMock<interface> imock;
  struct mock_extended_interface : extended_interface {
    MOCK_CONST_METHOD0(get, bool());
    MOCK_METHOD1(foo, void(bool));
    MOCK_METHOD1(bar, void(bool));
  } emock;

  EXPECT_CALL(imock, (get)()).WillOnce(Return(false));
  EXPECT_CALL(emock, bar(false)).Times(1);

  example e{imock, emock};
  e.update();
}

TEST(GMockVptr, ShouldNotCompileWhenMethodParametersDontMatch) {
  using namespace testing;
  GMock<interface> mock;
  //EXPECT_CALL(mock, (get)(42)).WillOnce(Return(42)); // COMPILE ERROR
}

TEST(GMockVptr, ShouldNotCompileWhenGMockNotUsedWithBrackets) {
  using namespace testing;
  GMock<interface> mock;
  //EXPECT_CALL(mock, get()).WillOnce(Return(42)); // COMPILE ERROR
}
