/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "LogicalClassificator.generated.hpp"

namespace multisetCheckModule
{

class LogicalClassificator : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

  void PerformInference(ScAddr inputStructure, ScAddr ruleSet, ScAddr targetTemplate);

  ScAddr FindInputStructure(ScAddr otherAddr);

  ScAddr FindInputLink(ScAddr inputStructure);

  ScAddr FindSetTranslationResultStructure(ScAddr inputStructure);

  ScAddr FindRuleSet(ScAddr otherAddr);

  ScAddr FindTargetTemplate(ScAddr otherAddr);

  ScAddr GetArguments(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, ScType fourthElementType, ScAddr fifthElement, int elementIndex);
};

} // namespace multisetCheckModule
