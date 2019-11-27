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

#include "Path.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace dtoql {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Component

Path::Component::Component(ComponentType type)
  : m_type(type)
{}

Path::ComponentType Path::Component::getType() {
  return m_type;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FieldCollection

Path::FieldCollection::FieldCollection(const std::vector<oatpp::String>& fields)
  : Component(ComponentType::FIELD_COLLECTION)
  , m_fields(fields)
{}

const std::vector<oatpp::String>& Path::FieldCollection::getFields() {
  return m_fields;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FieldCollection

Path::Variable::Variable(const oatpp::String& name)
  : Component(ComponentType::VARIABLE)
  , m_name(name)
{}

oatpp::String Path::Variable::getName() {
  return m_name;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Path

Path::Path(const std::list<std::shared_ptr<Component>>& components)
  : m_components(components)
{}

const std::list<std::shared_ptr<Path::Component>>& Path::getComponents() {
  return m_components;
}

oatpp::String Path::toString() {

  oatpp::data::stream::BufferOutputStream stream;

  for(auto& component : m_components) {

    switch(component->getType()) {

      case RE_ROOT: stream << "/"; break;
      case FIELD_SELECTOR: stream << "."; break;

      case FIELD_COLLECTION: {
        const auto &fields = std::static_pointer_cast<FieldCollection>(component)->getFields();
        stream << "[";
        for(v_int32 i = 0; i < fields.size(); i ++) {
          stream << "'" << fields[i] << "'";
          if(i < fields.size() - 1) {
            stream << ", ";
          }
        }
        stream << "]";
        break;
      }

      case VARIABLE: {
        auto name = std::static_pointer_cast<Variable>(component)->getName();
        if (name) {
          stream << "$" << name;
        } else {
          stream << "*";
        }
        break;
      }

    }

  }

  return stream.toString();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Builder

Path::Builder& Path::Builder::selectFields() {
  m_components.push_back(std::make_shared<FieldSelector>());
  return *this;
}

Path::Builder& Path::Builder::reRoot() {
  m_components.push_back(std::make_shared<ReRoot>());
  return *this;
}

Path::Builder& Path::Builder::fields(const std::vector<oatpp::String>& fieldNames) {
  m_components.push_back(std::make_shared<FieldCollection>(fieldNames));
  return *this;
}

Path::Builder& Path::Builder::variable(const oatpp::String& name) {
  m_components.push_back(std::make_shared<Variable>(name));
  return *this;
}

Path Path::Builder::build() {
  return Path(m_components);
}

std::shared_ptr<Path> Path::Builder::buildShared() {
  return std::make_shared<Path>(m_components);
}

}}