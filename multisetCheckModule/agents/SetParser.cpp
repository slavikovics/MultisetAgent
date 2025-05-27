#include <iostream>
#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>
#include <sc-memory/sc_template_search.cpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>

#include "SetParser.hpp"

using namespace std;
using namespace utils;

namespace multisetCheckModule
{
SC_AGENT_IMPLEMENTATION(SetParser)
{
  if (!m_memoryCtx.HelperCheckEdge(Keynodes::question_set_parsing, otherAddr, ScType::EdgeAccessConstPosPerm))
  {
    SC_LOG_DEBUG("SetParser: skipped, no edge from agent class.");
    return SC_RESULT_OK;
  }

  SC_LOG_DEBUG("SetParser: agent started.");

  ScAddr foundLink = FindInputStructure(otherAddr);
  std::string content = FindLinkContent(foundLink);

  std::vector<std::string> parsedElements = ParseSetElements(content);
  LogAgentActions(parsedElements);

  ScAddr structNode = CreateElementsInKnowledgeBase(parsedElements);
  CreateResultEdge(foundLink, structNode);

  SC_LOG_DEBUG("SetParser: successfully finished.");
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, otherAddr, true);

  return SC_RESULT_OK;
}

std::string SetParser::FindLinkContent(ScAddr otherAddr)
{
  std::string content;
  m_memoryCtx.GetLinkContent(otherAddr, content);
  SC_LOG_DEBUG("SetParser: link's content: " + content);

  return content;
}

ScAddr SetParser::FindInputStructure(ScAddr otherAddr)
{
  ScAddr inputStructure = GetArguments(
    otherAddr, 
    ScType::EdgeAccessConstPosPerm, 
    ScType::NodeConstStruct, 
    ScType::EdgeAccessConstPosPerm, 
    Keynodes::rrel_1, 
    2
  );

  ScIterator3Ptr const it3 = m_memoryCtx.Iterator3(
    inputStructure,
    ScType::EdgeAccessConstPosPerm,
    ScType::LinkConst
  );

  it3->Next();
  ScAddr foundLink = it3->Get(2);

  if (m_memoryCtx.GetElementType(foundLink) != ScType::LinkConst)
  {
    SC_LOG_ERROR("SetParser: link with a set was not found in input structure. Input structure cannot contain any other elements.");
  }

  SC_LOG_DEBUG("SetParser: input structure (sc-link) found.");
  return foundLink;
}

std::string SetParser::TrimContent(std::string content)
{
  size_t start = content.find_first_not_of(" \t\n\r");
  if (start == string::npos) return "";

  size_t end = content.find_last_not_of(" \t\n\r");
  return content.substr(start, end - start + 1);
}

std::vector<std::string> SetParser::ParseSetElements(std::string linkContent)
{
  std::vector<std::string> elements;
  std::string trimmed = TrimContent(linkContent);

  if (trimmed.empty() || trimmed.front() != '{' || trimmed.back() != '}')
  {
    return elements;
  }

  string inner = TrimContent(trimmed.substr(1, trimmed.size() -2));
  int braceLevel = 0;
  size_t startPos = 0;

  for (size_t i = 0; i < inner.size(); ++i)
  {
    char c = inner[i];

    if (c == '{' || c == '<') braceLevel++;
    else if (c == '}' || c == '>') braceLevel--;

    if (c == ',' && braceLevel == 0)
    {
      std::string element = TrimContent(inner.substr(startPos, i - startPos));

      if (!element.empty())
      {
        elements.push_back(element);
      } 

      startPos = i + 1;
    }
  }

  std::string lastElement = TrimContent(inner.substr(startPos));
  if (!lastElement.empty())
  {
    elements.push_back(lastElement);
  }

  return elements;
}

ScAddr SetParser::CreateElementsInKnowledgeBase(std::vector<std::string> elements)
{
  std::vector<std::string> alreadyCreatedValues;
  std::vector<ScAddr> alreadyCreatedNodes;

  ScAddr structNode = m_memoryCtx.CreateNode(ScType::NodeConstStruct);
  SC_LOG_DEBUG("SetParser: created struct node.");

  ScAddr setNode = m_memoryCtx.CreateNode(ScType::NodeConstClass);
  SC_LOG_DEBUG("SetParser: created set node.");

  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structNode, setNode);
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structNode, Keynodes::concept_element_with_multiple_entrances);

  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structNode, Keynodes::concept_set_to_check);
  ScAddr setToCheckEdge = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::concept_set_to_check, setNode);
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structNode, setToCheckEdge);

  for (std::string elementContent : elements)
  {
    int checkResult = CheckIfElementIsAlreadyinSet(alreadyCreatedValues, elementContent);

    if (checkResult != -1)
    {
      SC_LOG_DEBUG("SetParser: creating edge for element with multiple entrances " + elementContent);
      ScAddr foundNode = GetScAddrOfElement(alreadyCreatedNodes, checkResult);

      if (!m_memoryCtx.HelperCheckEdge(Keynodes::concept_element_with_multiple_entrances, foundNode, ScType::EdgeAccessConstPosPerm))
      {
        ScAddr edge = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::concept_element_with_multiple_entrances, foundNode);
        m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structNode, edge);
      }

      continue;
    }

    ScAddr elementNode = m_memoryCtx.HelperResolveSystemIdtf(elementContent, ScType::NodeConst);
   
    alreadyCreatedNodes.push_back(elementNode);
    alreadyCreatedValues.push_back(elementContent);

    ScAddr elementEdge = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, setNode, elementNode);

    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structNode, elementNode);
    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, structNode, elementEdge);

    SC_LOG_DEBUG("SetParser: added element into a set: " + elementContent);
  }

  m_memoryCtx.Save();
  return structNode;
}

ScAddr SetParser::GetArguments(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, ScType fourthElementType, ScAddr fifthElement, int elementIndex)
{
  ScIterator5Ptr const argumentsIterator = m_memoryCtx.Iterator5(
    firstElement,
    secondElementType,
    thirdElementType,
    fourthElementType,
    fifthElement
  );

  argumentsIterator->Next();
  return argumentsIterator->Get(elementIndex);
}

void SetParser::CreateResultEdge(ScAddr otherAddr, ScAddr resultStruct)
{
  ScAddr edge = m_memoryCtx.CreateEdge(ScType::EdgeDCommonConst, otherAddr, resultStruct);
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_translated_set_structure, edge);
  SC_LOG_DEBUG("SetParser: result edge created.");

  m_memoryCtx.Save();
}

void SetParser::LogAgentActions(std::vector<std::string> elements)
{
  SC_LOG_DEBUG("SetParser: following elements will be created: ");

  for (std::string element : elements)
  {
    SC_LOG_DEBUG("SetParser: element " + element);
  }
}

int SetParser::CheckIfElementIsAlreadyinSet(std::vector<std::string> elements, std::string element)
{
  if (elements.size() == 0) return -1;
  int i = 0;

  for (std::string el : elements)
  {
    if (el == element) return i;
    i++;
  }

  return -1;
}

ScAddr SetParser::GetScAddrOfElement(std::vector<ScAddr> elements, int i)
{
  int current = 0;

  for (ScAddr addr : elements)
  {
    if (i == current) return addr;
    current++;
  }

  return ScAddr();
}

}  // namespace multisetCheckModule
