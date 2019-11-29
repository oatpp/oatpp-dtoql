/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "Traverser.hpp"

namespace oatpp { namespace dtoql {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Field

Traverser::Field::Field(const oatpp::String& name, v_int64 index, const AbstractObjectWrapper& value)
  : m_name(name)
  , m_index(index)
  , m_value(value)
{}

oatpp::String Traverser::Field::getName() const {
  return m_name;
}

v_int64 Traverser::Field::getIndex() const {
  return m_index;
}

Traverser::AbstractObjectWrapper Traverser::Field::getValue() const {
  return m_value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Traverser

Traverser::Traverser(const std::shared_ptr<Path>& path, const AbstractObjectWrapper& polymorph)
  : m_path(path)
  , m_object(polymorph)
{}

std::list<Traverser::Field> Traverser::selectFieldsInList(const AbstractList::ObjectWrapper& list, const std::shared_ptr<Path::FieldCollection>& fields) {

  std::list<Field> result;

  for(const auto& f : fields->getFields()) {
    if(f.getType() == Path::FieldReference::Type::INDEX) {
      auto value = list->get(f.getIndex());
      if(value) {
        result.push_back(Field(nullptr, f.getIndex(), value));
      }
    }
  }

  return result;

}

std::list<Traverser::Field> Traverser::selectFieldsInMap(const AbstractFieldsMap::ObjectWrapper& map, const std::shared_ptr<Path::FieldCollection>& fields) {

  std::list<Field> result;

  for(const auto& f : fields->getFields()) {

    if(f.getType() == Path::FieldReference::Type::INDEX) {

      auto entry = map->getEntryByIndex(f.getIndex());
      if(entry) {
        result.push_back(Field(entry->getKey(), f.getIndex(), entry->getValue()));
      }

    } else if(f.getType() == Path::FieldReference::Type::NAME) {

      auto currEntry = map->getFirstEntry();
      v_int64 index = 0;

      while(currEntry != nullptr) {

        if(f.getName() == currEntry->getKey()) {
          result.push_back(Field(currEntry->getKey(), index, currEntry->getValue()));
          break;
        }

        index ++;
        currEntry = currEntry->getNext();
      }

    }

  }

  return result;

}

std::list<Traverser::Field> Traverser::selectFieldsInObject(const PolymorphicWrapper<Object>& polymorph, const std::shared_ptr<Path::FieldCollection>& fields) {

  std::list<Field> result;

  Object* object = polymorph.get();

  for(const auto& f : fields->getFields()) {

    if(f.getType() == Path::FieldReference::Type::INDEX) {

      auto fields = polymorph.valueType->properties->getList();
      v_int64 index = 0;

      for (auto const& field : fields) {
        if(index == f.getIndex()) {
          auto value = field->get(object);
          result.push_back(Field(field->name, index, value));
          break;
        }
        index ++;
      }

    } else if(f.getType() == Path::FieldReference::Type::NAME) {

      const auto& fields = polymorph.valueType->properties->getMap();
      auto it = fields.find(f.getName()->std_str());
      if(it != fields.end()) {
        auto value = it->second->get(object);
        result.push_back(Field(f.getName(), -1, value));
      }

    }

  }

  return result;

}

std::list<Traverser::Field> Traverser::selectFields(const AbstractObjectWrapper& polymorph, const std::shared_ptr<Path::FieldCollection>& fields) {

  auto type = polymorph.valueType;

  if(type->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
    // List
    return selectFieldsInList(oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(polymorph), fields);
  } else if(type->name == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {
    // Map
    return selectFieldsInMap(oatpp::data::mapping::type::static_wrapper_cast<AbstractFieldsMap>(polymorph), fields);
  } else if(type->name == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
    // Object
    return selectFieldsInObject(oatpp::data::mapping::type::static_wrapper_cast<Object>(polymorph), fields);
  }

  return std::list<Traverser::Field>();

}

bool Traverser::iterate() {



  return false;

}

}}
