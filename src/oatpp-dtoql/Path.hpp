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

#ifndef oatpp_dtoql_Path_hpp
#define oatpp_dtoql_Path_hpp

#include "oatpp/core/Types.hpp"
#include <vector>

namespace oatpp { namespace dtoql {

class Path {
public:

  enum ComponentType : v_int32 {
    RE_ROOT = 0,
    FIELD_SELECTOR = 1,
    FIELD_COLLECTION = 2,
    VARIABLE = 3
  };

  class Component {
  private:
    ComponentType m_type;
  public:

    Component(ComponentType type);

    virtual ~Component() = default;

    ComponentType getType();

  };

  class ReRoot : public Component {
  public:
    ReRoot() : Component(ComponentType::RE_ROOT) {}
  };

  class FieldSelector : public Component {
  public:
    FieldSelector() : Component(ComponentType::FIELD_SELECTOR) {}
  };

  class FieldReference {
  public:
    enum Type : v_int32 {
      NAME = 0,
      INDEX = 1
    };
  private:
    oatpp::String m_name;
    v_int64 m_index;
    Type m_type;
  public:

    FieldReference(const oatpp::String& name);
    FieldReference(const char* name) : FieldReference(oatpp::String(name)) {};
    FieldReference(v_int64 index);

    oatpp::String getName() const;
    v_int64 getIndex() const;
    Type getType() const;

  };

  class FieldCollection : public Component {
  private:
    std::vector<FieldReference> m_fields;
  public:

    FieldCollection(const std::vector<FieldReference>& fields);
    const std::vector<FieldReference>& getFields();

  };

  class Variable : public Component {
  private:
    oatpp::String m_name;
  public:

    Variable(const oatpp::String& name);
    oatpp::String getName();

  };

private:
  std::vector<std::shared_ptr<Component>> m_components;
public:

  Path(const std::vector<std::shared_ptr<Component>>& components);

  const std::vector<std::shared_ptr<Component>>& getComponents();

  oatpp::String toString();

public:

  class Builder {
  private:
    std::vector<std::shared_ptr<Component>> m_components;
  public:

    Builder& selectFields();

    Builder& reRoot();

    Builder& fields(const std::vector<FieldReference>& fieldReferences);

    Builder& variable(const oatpp::String& name);

    Path build();

    std::shared_ptr<Path> buildShared();

  };

};

}}

#endif // oatpp_dtoql_Path_hpp
