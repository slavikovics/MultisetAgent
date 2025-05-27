/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "MultisetCheckModule.hpp"
#include "keynodes/keynodes.hpp"
#include "agents/SetParser.hpp"
#include "agents/LogicalClassificator.hpp"

using namespace multisetCheckModule;

SC_IMPLEMENT_MODULE(MultisetCheckModule)

sc_result MultisetCheckModule::InitializeImpl()
{
  if (!multisetCheckModule::Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(SetParser)
  SC_AGENT_REGISTER(LogicalClassificator)

  return SC_RESULT_OK;
}

sc_result MultisetCheckModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(SetParser)
  SC_AGENT_UNREGISTER(LogicalClassificator)

  return SC_RESULT_OK;
}
