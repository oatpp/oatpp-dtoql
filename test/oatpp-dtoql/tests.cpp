
#include "oatpp-test/UnitTest.hpp"

#include "oatpp-dtoql/Path.hpp"

#include <iostream>

namespace {

class Test : public oatpp::test::UnitTest {
public:
  Test() : oatpp::test::UnitTest("MyTag")
  {}

  void onRun() override {

    {
      auto path = oatpp::dtoql::Path::Builder()
        .selectFields().fields({"phoneNumbers"})
        .reRoot()
        .variable(nullptr)
        .selectFields().fields({"type", "number", 12})
        .build();

      auto str = path.toString();
      OATPP_LOGD("path", "\"%s\"", str->getData());
    }

  }
};

void runTests() {
  OATPP_RUN_TEST(Test);
}

}

int main() {

  oatpp::base::Environment::init();

  runTests();

  /* Print how much objects were created during app running, and what have left-probably leaked */
  /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

  OATPP_ASSERT(oatpp::base::Environment::getObjectsCount() == 0);

  oatpp::base::Environment::destroy();

  return 0;
}
