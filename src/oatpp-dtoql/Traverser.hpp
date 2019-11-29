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

#ifndef oatpp_dtoql_Traverser_hpp
#define oatpp_dtoql_Traverser_hpp

#include "./Path.hpp"

#include "oatpp/core/data/mapping/type/ListMap.hpp"
#include "oatpp/core/data/mapping/type/List.hpp"
#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/data/mapping/type/Type.hpp"

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace dtoql {

class Traverser {
public:
  typedef oatpp::data::mapping::type::Type Type;
  typedef oatpp::data::mapping::type::Type::Property Property;
  typedef oatpp::data::mapping::type::Type::Properties Properties;

  typedef oatpp::data::mapping::type::Object Object;
  typedef oatpp::String String;

  template<class T>
  using PolymorphicWrapper = data::mapping::type::PolymorphicWrapper<T>;

  typedef oatpp::data::mapping::type::AbstractObjectWrapper AbstractObjectWrapper;
  typedef oatpp::data::mapping::type::List<AbstractObjectWrapper> AbstractList;
  typedef oatpp::data::mapping::type::ListMap<String, AbstractObjectWrapper> AbstractFieldsMap;

public:

  class Field {
  private:
    oatpp::String m_name;
    v_int64 m_index;
    AbstractObjectWrapper m_value;
  public:

    Field(const oatpp::String& name, v_int64 index, const AbstractObjectWrapper& value);

    oatpp::String getName() const;
    v_int64 getIndex() const;
    AbstractObjectWrapper getValue() const;

  };

private:
  std::shared_ptr<Path> m_path;
  AbstractObjectWrapper m_object;
private:

  AbstractObjectWrapper m_currObject;

private:

  static std::list<Field> selectFieldsInList(const AbstractList::ObjectWrapper& list, const std::shared_ptr<Path::FieldCollection>& fields);
  static std::list<Field> selectFieldsInMap(const AbstractFieldsMap::ObjectWrapper& map, const std::shared_ptr<Path::FieldCollection>& fields);
  static std::list<Field> selectFieldsInObject(const PolymorphicWrapper<Object>& polymorph, const std::shared_ptr<Path::FieldCollection>& fields);
public:
  static std::list<Field> selectFields(const AbstractObjectWrapper& polymorph, const std::shared_ptr<Path::FieldCollection>& fields);

public:

  Traverser(const std::shared_ptr<Path>& path, const AbstractObjectWrapper& polymorph);


  bool iterate();

};

}}

#endif // oatpp_dtoql_Traverser_hpp
