#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "SetParser.generated.hpp"

namespace multisetCheckModule
{
class SetParser : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

  std::string FindLinkContent(ScAddr otherAddr);

  ScAddr FindInputStructure(ScAddr otherAddr);

  ScAddr CreateElementsInKnowledgeBase(std::vector<std::string> elements);

  std::vector<std::string> ParseSetElements(std::string linkContent);

  std::string TrimContent(std::string content);

  int CheckIfElementIsAlreadyinSet(std::vector<std::string> elements, std::string element);

  ScAddr GetScAddrOfElement(std::vector<ScAddr> elements, int i);

  void CreateResultEdge(ScAddr otherAddr, ScAddr resultStruct);

  void LogAgentActions(std::vector<std::string> elements);

  ScAddr GetArguments(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, ScType fourthElementType, ScAddr fifthElement, int elementIndex);
};

}  // namespace multisetCheckModule
