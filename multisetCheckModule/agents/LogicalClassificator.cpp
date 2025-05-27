/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include <unistd.h>

#include <factory/InferenceManagerFactory.hpp>

#include "LogicalClassificator.hpp"

using namespace std;
using namespace utils;
using namespace inference;

namespace multisetCheckModule
{

SC_AGENT_IMPLEMENTATION(LogicalClassificator)
{
  if (!m_memoryCtx.HelperCheckEdge(Keynodes::question_logical_classification, otherAddr, ScType::EdgeAccessConstPosPerm))
  {
    SC_LOG_DEBUG("LogicalClassificator: skipped, no edge from agent class.");
    return SC_RESULT_OK;
  }

  SC_LOG_DEBUG("LogicalClassificator: agent started");
  sleep(2);

  ScAddr inputStructure = FindInputStructure(otherAddr);
  ScAddr inputLink = FindInputLink(inputStructure);
  ScAddr targetTemplate = FindTargetTemplate(otherAddr);
  ScAddr ruleSet = FindRuleSet(otherAddr);
  SC_LOG_DEBUG("LogicalClassificator: arguments accepted.");

  ScAddr inputLinkTranslationResult = FindSetTranslationResultStructure(inputLink);
  PerformInference(inputLinkTranslationResult, ruleSet, targetTemplate);

  SC_LOG_DEBUG("LogicalClassificator: successfully finished.");
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, otherAddr, true);
  return SC_RESULT_OK;
}

void LogicalClassificator::PerformInference(ScAddr inputStructure, ScAddr ruleSet, ScAddr targetTemplate)
{
  SC_LOG_DEBUG("Inference started");
  ScAddr const & output_structure = m_memoryCtx.CreateNode(ScType::NodeConstStruct);
  InferenceParams const & inferenceParams {ruleSet, {}, {inputStructure}, output_structure, targetTemplate};

  InferenceConfig const & inferenceConfig {GENERATE_UNIQUE_FORMULAS, REPLACEMENTS_ALL, TREE_ONLY_OUTPUT_STRUCTURE, SEARCH_IN_STRUCTURES};
  std::unique_ptr<InferenceManagerAbstract> inferenceManager = InferenceManagerFactory::constructDirectInferenceManagerTarget(&m_memoryCtx, inferenceConfig);

  bool targetAchieved = inferenceManager->applyInference(inferenceParams);
  SC_LOG_INFO("Target achieved: " + std::to_string(targetAchieved));

  ScAddr answer = inferenceManager->getSolutionTreeManager()->createSolution(output_structure, targetAchieved);
}

ScAddr LogicalClassificator::FindInputStructure(ScAddr otherAddr)
{
  ScAddr inputStructure = GetArguments(
    otherAddr,
    ScType::EdgeAccessConstPosPerm, 
    ScType::NodeConstStruct, 
    ScType::EdgeAccessConstPosPerm, 
    Keynodes::rrel_1, 
    2
  );

  return inputStructure;
}

ScAddr LogicalClassificator::FindInputLink(ScAddr inputStructure)
{
  ScIterator3Ptr const it3 = m_memoryCtx.Iterator3(
    inputStructure,
    ScType::EdgeAccessConstPosPerm,
    ScType::LinkConst
  );

  it3->Next();
  return it3->Get(2);
}

ScAddr LogicalClassificator::FindSetTranslationResultStructure(ScAddr inputLink)
{
  ScAddr inputLinkTranslationResult = GetArguments(
    inputLink,
    ScType::EdgeDCommonConst,
    ScType::NodeConstStruct,
    ScType::EdgeAccessConstPosPerm,
    Keynodes::nrel_translated_set_structure,
    2
  );

  return inputLinkTranslationResult;
}

ScAddr LogicalClassificator::FindRuleSet(ScAddr otherAddr)
{
  ScAddr ruleSet = GetArguments(
    otherAddr,
    ScType::EdgeAccessConstPosPerm, 
    ScType::NodeConst, 
    ScType::EdgeAccessConstPosPerm, 
    Keynodes::rrel_2, 
    2
  );

  SC_LOG_DEBUG("Found rule set: " + m_memoryCtx.HelperGetSystemIdtf(ruleSet));
  return ruleSet;
}

ScAddr LogicalClassificator::FindTargetTemplate(ScAddr otherAddr)
{
  ScAddr targetTemplate = GetArguments(
    otherAddr,
    ScType::EdgeAccessConstPosPerm, 
    ScType::NodeConstStruct, 
    ScType::EdgeAccessConstPosPerm, 
    Keynodes::rrel_3, 
    2
  );

  SC_LOG_DEBUG("Found target template: " + m_memoryCtx.HelperGetSystemIdtf(targetTemplate));
  return targetTemplate;
}

ScAddr LogicalClassificator::GetArguments(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, ScType fourthElementType, ScAddr fifthElement, int elementIndex)
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
}
