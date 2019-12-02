
#include "oatpp-test/UnitTest.hpp"

#include "oatpp-dtoql/Traverser.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include <iostream>

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class DtoLevel3 : public oatpp::data::mapping::type::Object {

  DTO_INIT(DtoLevel3, Object)

  DTO_FIELD(String, str_value);
  DTO_FIELD(Int32, int_value);
  DTO_FIELD(Boolean, bool_value);

};

class DtoLevel2 : public oatpp::data::mapping::type::Object {

  DTO_INIT(DtoLevel2, Object)

  DTO_FIELD(List<DtoLevel3::ObjectWrapper>::ObjectWrapper, list);
  DTO_FIELD(Fields<DtoLevel3::ObjectWrapper>::ObjectWrapper, map);

};

class DtoLevel1 : public oatpp::data::mapping::type::Object {

  DTO_INIT(DtoLevel1, Object)

  DTO_FIELD(DtoLevel2::ObjectWrapper, child1);
  DTO_FIELD(DtoLevel2::ObjectWrapper, child2);

};

#include OATPP_CODEGEN_END(DTO)

class Test : public oatpp::test::UnitTest {
public:
  Test() : oatpp::test::UnitTest("MyTag")
  {}

  DtoLevel1::ObjectWrapper createTestDto() {

    auto dto = DtoLevel1::createShared();
    dto->child1 = DtoLevel2::createShared();
    dto->child2 = DtoLevel2::createShared();

    dto->child1->list = dto->child1->list->createShared();
    dto->child1->map = dto->child1->map->createShared();

    dto->child2->list = dto->child1->list->createShared();
    dto->child2->map = dto->child1->map->createShared();

    {
      for (v_int32 i = 0; i < 10; i++) {
        auto obj = DtoLevel3::createShared();

        obj->str_value = "Str." + oatpp::utils::conversion::int32ToStr(i);
        obj->int_value = i;
        obj->bool_value = true;

        dto->child1->list->pushBack(obj);
        dto->child1->map->put("Key." + oatpp::utils::conversion::int32ToStr(i), obj);
      }
    }

    {
      for (v_int32 i = 0; i < 10; i++) {
        auto obj = DtoLevel3::createShared();

        obj->str_value = "StrValue2." + oatpp::utils::conversion::int32ToStr(i);
        obj->int_value = 1000 + i;
        obj->bool_value = false;

        dto->child2->list->pushBack(obj);
        dto->child2->map->put("Key-Obj-2." + oatpp::utils::conversion::int32ToStr(i), obj);
      }
    }

    return dto;

  }

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

    {
//      auto dto = createTestDto();
//
//      auto collection = std::shared_ptr<oatpp::dtoql::Path::FieldCollection>(nullptr);//new oatpp::dtoql::Path::FieldCollection({"Key.0", 1, 2}));
//
//      auto selection = oatpp::dtoql::Traverser::selectFields(dto->child2->list, collection);
//
//      auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
//
//      for(auto& field : selection) {
//
//        auto json = objectMapper->writeToString(field.getValue());
//
//        if(field.getName()) {
//          OATPP_LOGD("Field", "index=%d, name='%s', value='%s'", field.getIndex(), field.getName()->getData(), json->getData());
//        } else {
//          OATPP_LOGD("Field", "index=%d, name='%s', value='%s'", field.getIndex(), "<null>", json->getData());
//        }
//
//      }

    }

    {

      auto path = oatpp::dtoql::Path::Builder()
        .variable(nullptr)
        .fields({"list", "map"})
        .fields({v_int64(0), "Key-Obj-2.5", 9})
        .fields({"int_value"})
        .buildShared();

      auto str = path->toString();
      OATPP_LOGD("path", "\"%s\"", str->getData());

      auto dto = createTestDto();

      oatpp::dtoql::Traverser traverser(path, dto);

      while(traverser.iterate()){
        traverser.printResultTable();
      }

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
