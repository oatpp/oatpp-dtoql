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

#include <iostream>

namespace oatpp { namespace dtoql {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Field

Traverser::Field::Field()
  : m_name(nullptr)
  , m_index(-1)
  , m_value(nullptr)
  , m_isValid(false)
{}

Traverser::Field::Field(const oatpp::String& name, v_int64 index, const AbstractObjectWrapper& value)
  : m_name(name)
  , m_index(index)
  , m_value(value)
  , m_isValid(true)
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
// StackNode

Traverser::StackNode::StackNode(std::list<Field>&& set)
  : m_set(std::forward<std::list<Field>>(set))
{}

std::list<Traverser::Field> Traverser::StackNode::popNextSelection(const std::shared_ptr<Path::FieldCollection>& fields) {
  m_currField = m_set.front();
  m_set.pop_front();
  return selectFields(m_currField.getValue(), fields);
}

const Traverser::Field& Traverser::StackNode::popNext() {
  m_currField = m_set.front();
  m_set.pop_front();
  return m_currField;
}

const Traverser::Field& Traverser::StackNode::getCurrentField() {
  return m_currField;
}

bool Traverser::StackNode::isEmpty() {
  return m_set.empty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Traverser

Traverser::Traverser(const std::shared_ptr<Path>& path, const AbstractObjectWrapper& polymorph)
  : m_path(path)
  , m_pathComponentIndex(0)
{
  std::list<Field> initialSet;
  initialSet.push_back(Field(nullptr, 0, polymorph));
  m_stack.push_back(std::make_shared<StackNode>(std::move(initialSet)));
}

std::list<Traverser::Field> Traverser::selectFieldsInList(const AbstractList::ObjectWrapper& list, const std::shared_ptr<Path::FieldCollection>& fields) {

  std::list<Field> result;

  if(fields) {

    for (const auto &f : fields->getFields()) {
      if (f.getType() == Path::FieldReference::Type::INDEX) {
        auto node = list->getNode(f.getIndex());
        if (node && node->getData()) {
          result.push_back(Field(nullptr, f.getIndex(), node->getData()));
        }
      }
    }

  } else {

    auto node = list->getFirstNode();
    v_int64 index = 0;
    while(node != nullptr) {
      auto data = node->getData();
      if(data) {
        result.push_back(Field(nullptr, index, data));
      }
      index ++;
      node = node->getNext();
    }

  }

  return result;

}

std::list<Traverser::Field> Traverser::selectFieldsInMap(const AbstractFieldsMap::ObjectWrapper& map, const std::shared_ptr<Path::FieldCollection>& fields) {

  std::list<Field> result;

  if(fields) {

    for (const auto &f : fields->getFields()) {

      if (f.getType() == Path::FieldReference::Type::INDEX) {

        auto entry = map->getEntryByIndex(f.getIndex());
        if (entry) {
          result.push_back(Field(entry->getKey(), f.getIndex(), entry->getValue()));
        }

      } else if (f.getType() == Path::FieldReference::Type::NAME) {

        auto currEntry = map->getFirstEntry();
        v_int64 index = 0;

        while (currEntry != nullptr) {

          if (f.getName() == currEntry->getKey()) {
            result.push_back(Field(currEntry->getKey(), index, currEntry->getValue()));
            break;
          }

          index ++;
          currEntry = currEntry->getNext();
        }

      }

    }

  } else {

    auto currEntry = map->getFirstEntry();
    v_int64 index = 0;
    while(currEntry != nullptr) {
      result.push_back(Field(currEntry->getKey(), index, currEntry->getValue()));
      index ++;
      currEntry = currEntry->getNext();
    }

  }

  return result;

}

std::list<Traverser::Field> Traverser::selectFieldsInObject(const PolymorphicWrapper<Object>& polymorph, const std::shared_ptr<Path::FieldCollection>& fields) {

  std::list<Field> result;

  Object* object = polymorph.get();

  if(fields) {

    for (const auto &f : fields->getFields()) {

      if (f.getType() == Path::FieldReference::Type::INDEX) {

        auto fields = polymorph.valueType->properties->getList();
        v_int64 index = 0;

        for (auto const &field : fields) {
          if (index == f.getIndex()) {
            auto value = field->get(object);
            result.push_back(Field(field->name, index, value));
            break;
          }
          index++;
        }

      } else if (f.getType() == Path::FieldReference::Type::NAME) {

        const auto &fields = polymorph.valueType->properties->getMap();
        auto it = fields.find(f.getName()->std_str());
        if (it != fields.end()) {
          auto value = it->second->get(object);
          result.push_back(Field(f.getName(), -1, value));
        }

      }

    }

  } else {

    auto fields = polymorph.valueType->properties->getList();

    v_int64 index = 0;
    for (auto const &field : fields) {
      auto value = field->get(object);
      result.push_back(Field(field->name, index, value));
      index++;
    }

  }

  return result;

}

std::list<Traverser::Field> Traverser::selectFields(const AbstractObjectWrapper& polymorph, const std::shared_ptr<Path::FieldCollection>& fields) {

  auto classId = polymorph.valueType->classId.id;

  if(classId == oatpp::data::mapping::type::__class::AbstractList::CLASS_ID.id) {
    // List
    return selectFieldsInList(oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(polymorph), fields);
  } else if(classId == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_ID.id) {
    // Map
    return selectFieldsInMap(oatpp::data::mapping::type::static_wrapper_cast<AbstractFieldsMap>(polymorph), fields);
  } else if(classId == oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID.id) {
    // Object
    return selectFieldsInObject(oatpp::data::mapping::type::static_wrapper_cast<Object>(polymorph), fields);
  }

  return std::list<Traverser::Field>();

}

void Traverser::pushResult() {

  std::vector<Field> row;

  for(const auto& stackNode : m_stack) {
    row.push_back(stackNode->getCurrentField());
  }

  m_resultTable.push_back(std::move(row));

}

bool Traverser::iterate() {

  if(m_stack.empty()) {
    return false;
  }

  auto currStackNode = m_stack.back();

  if(currStackNode->isEmpty()) {
    m_stack.pop_back();
    m_pathComponentIndex --;
  } else if(m_pathComponentIndex == m_path->getComponents().size()) {
    currStackNode->popNext();
    pushResult();
  } else {

    auto component = m_path->getComponents()[m_pathComponentIndex];

    switch (component->getType()) {

      case Path::ComponentType::FIELD_COLLECTION: {
        const auto &fields = std::static_pointer_cast<Path::FieldCollection>(component);
        auto selection = currStackNode->popNextSelection(fields);
        m_stack.push_back(std::make_shared<StackNode>(std::move(selection)));
        break;
      }

      case Path::ComponentType::VARIABLE: {
        auto selection = currStackNode->popNextSelection(nullptr);
        m_stack.push_back(std::make_shared<StackNode>(std::move(selection)));
        break;
      }

      default:
        break;

    }

    m_pathComponentIndex++;

  }

  return true;

}

const std::vector<std::vector<Traverser::Field>>& Traverser::getResultTable() {
  return m_resultTable;
}

void Traverser::printResultTable() {

  std::cout << "results:\n";

  for(const auto& row : m_resultTable) {

    for(const auto& field : row) {
      if(field.getName()) {
        std::cout << field.getName()->c_str() << ".";
      } else {
        std::cout << "[" << field.getIndex() << "].";
      }
    }

    std::cout << "\n";

  }

  std::cout << std::endl;

}

}}
