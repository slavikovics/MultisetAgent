/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>

#include "keynodes.generated.hpp"

namespace multisetCheckModule
{
class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("question_logical_classification"), ForceCreate(ScType::NodeConstClass))
  static ScAddr question_logical_classification;

  SC_PROPERTY(Keynode("question_set_parsing"), ForceCreate(ScType::NodeConstClass))
  static ScAddr question_set_parsing;

  SC_PROPERTY(Keynode("nrel_translated_set_structure"), ForceCreate(ScType::NodeConstNoRole))
  static ScAddr nrel_translated_set_structure;

  SC_PROPERTY(Keynode("question_initiated"), ForceCreate(ScType::NodeConstClass))
  static ScAddr question_initiated;

  SC_PROPERTY(Keynode("rrel_1"), ForceCreate(ScType::NodeConstRole))
  static ScAddr rrel_1;

  SC_PROPERTY(Keynode("rrel_2"), ForceCreate(ScType::NodeConstRole))
  static ScAddr rrel_2;

  SC_PROPERTY(Keynode("rrel_3"), ForceCreate(ScType::NodeConstRole))
  static ScAddr rrel_3;

  SC_PROPERTY(Keynode("concept_element_with_multiple_entrances"), ForceCreate(ScType::NodeConstClass))
  static ScAddr concept_element_with_multiple_entrances;

  SC_PROPERTY(Keynode("concept_set_to_check"), ForceCreate(ScType::NodeConstClass))
  static ScAddr concept_set_to_check;
};

}  // namespace multisetCheckModule
