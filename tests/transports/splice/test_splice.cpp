
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge_engine/KnowledgeBase.h"
#include "madara/utility/LogMacros.h"

std::string host ("");
Madara::Transport::QoSTransportSettings settings;

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc)
        host = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.domains = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.id;
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> MADARA_debug_level;
      }

      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--drop-rate")
    {
      if (i + 1 < argc)
      {
        double drop_rate;
        std::stringstream buffer (argv[i + 1]);
        buffer >> drop_rate;
        
        settings.update_drop_rate (drop_rate,
          Madara::Transport::PACKET_DROP_DETERMINISTIC);
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        Madara::KnowledgeEngine::KnowledgeBase::log_to_file (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
        "\nProgram summary for %s:\n\n" \
        "  Test the Splice DDS transport. Requires 2+ processes. The result of\n" \
        "  running these processes should be that each process reports\n" \
        "  var2 and var3 being set to 1.\n\n" \
        " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
        " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
        " [-i|--id id]             the id of this agent (should be non-negative)\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        " [-f|--logfile file]      log to a file\n" \
        " [-r|--reduced]           use the reduced message header\n" \
        "\n",
        argv[0]));
      exit (0);
    }
  }
}



int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);

  settings.type = Madara::Transport::SPLICE;
  settings.reliability = Madara::Transport::RELIABLE;
  Madara::KnowledgeEngine::WaitSettings wait_settings;
  wait_settings.max_wait_time = 10;

  Madara::KnowledgeEngine::KnowledgeBase knowledge (host, settings);

  knowledge.set (".id", (Madara::KnowledgeRecord::Integer) settings.id);

  if (settings.id == 0)
  {
    Madara::KnowledgeEngine::CompiledExpression compiled = 
      knowledge.compile (
        "(var2 = 1) ;> (var1 = 0) ;> (var4 = -2.0/3) ;> var3"
      );

    knowledge.wait (compiled, wait_settings);
  }
  else
  {
    Madara::KnowledgeEngine::CompiledExpression compiled = 
      knowledge.compile ("!var1 && var2 => var3 = 1");

    knowledge.wait (compiled, wait_settings);
  }

  knowledge.print_knowledge ();

  return 0;
}