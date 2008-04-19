// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltscompare.cpp

#define NAME "ltscompare"
#define AUTHOR "Muck van Weerdenburg"

#include <string>
#include "aterm2.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/lts/liblts.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h" // after messaging.h and rewrite.h

using namespace std;
using namespace ::mcrl2::lts;
using namespace ::mcrl2::utilities;
using namespace mcrl2::core;

static const char *equivalent_string(lts_equivalence eq)
{
  switch ( eq )
  {
    case lts_eq_strong:
      return "strongly bisimilar";
    case lts_eq_branch:
      return "branching bisimilar";
    default:
      return "equivalent";
  }
}

static void print_formats(FILE *f)
{
  fprintf(f,
    "The following formats are accepted by " NAME ":\n"
    "\n"
    "  format  ext.  description                       remarks\n"
    "  -----------------------------------------------------------\n"
    "  aut     .aut  Aldebaran format (CADP)\n"
#ifdef MCRL2_BCG
    "  bcg     .bcg  Binary Coded Graph format (CADP)\n"
#endif
    "  mcrl    .svc  mCRL SVC format\n"
    "  mcrl2   .svc  mCRL2 SVC format                  default\n"
    "\n"
    );
}

struct t_tool_options {
  std::string     name_for_first;
  std::string     name_for_second;
  lts_type        format_for_first;
  lts_type        format_for_second;
  lts_equivalence equivalence;
  lts_eq_options  eq_opts;
};

t_tool_options parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE1] INFILE2\n"
    "Compare the labelled transition systems (LTSs) INFILE1 to INFILE2 in the"
    "requested format modulo a certain equivalence If INFILE1 is not supplied, stdin"
    "is used.\n"
    "\n"
    "The input formats are determined by the contents of INFILE1 and INFILE2. "
    "Options --in1 and --in2 can beused to force the input format of INFILE1 and "
    "INFILE2, respectively.");

  clinterface.
    add_option("formats", "list accepted formats", 'f').
    add_option("lps", make_mandatory_argument("FILE"),
      "use FILE as the LPS from which the input LTS was generated; this is "
      "needed to store the correct parameter names of states when saving "
      "in fsm format and to convert non-mCRL2 LTSs to a mCRL2 LTS", 'l').
    add_option("in1", make_mandatory_argument("FORMAT"),
      "use FORMAT as the format for INFIL1 (or stdin)", 'i').
    add_option("in2", make_mandatory_argument("FORMAT"),
      "use FORMAT as the format for INFILE2", 'j').
    add_option("strong",
      "use strong bisimulation equivalence (default)", 's').
    add_option("branching",
      "use branching bisimulation equivalence", 'b').
    add_option("tau", make_mandatory_argument("ACTNAMES"),
      "consider actions with a name in the comma separated list ACTNAMES to "
      "be internal (tau) actions in addition to those defined as such by "
      "the input");

  command_line_parser parser(clinterface, ac, av);

  t_tool_options tool_options;

  tool_options.equivalence = lts_eq_strong;

  if (parser.options.count("formats")) {
    print_formats(stderr);
  }
  if (parser.options.count("strong")) {
    tool_options.equivalence = lts_eq_strong;
  }
  if (parser.options.count("branching")) {
    tool_options.equivalence = lts_eq_branch;
  }
  if (parser.options.count("tau")) {
    lts_reduce_add_tau_actions(tool_options.eq_opts, parser.option_argument("tau"));
  }

  if (parser.arguments.size() == 0) {
    parser.error("need at least one file argument");
  }
  if (parser.arguments.size() == 1) {
    tool_options.format_for_first = lts_aut;
    tool_options.name_for_second  = parser.arguments[0];
  }
  if (1 < parser.arguments.size()) {
    tool_options.name_for_first  = parser.arguments[0];
    tool_options.name_for_second = parser.arguments[1];
  }
  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }

  if (parser.options.count("in1")) {
    if (1 < parser.options.count("in1")) {
      std::cerr << "warning: first input format has already been specified; extra option ignored\n";
    }

    tool_options.format_for_first = lts::parse_format(parser.option_argument("in1").c_str());

    if (tool_options.format_for_first == lts_none) {
      std::cerr << "warning: format '" << parser.option_argument("in1") <<
                   "' is not recognised; option ignored" << std::endl;
    }
  }
  else if (!tool_options.name_for_first.empty()) {
    tool_options.format_for_first = lts::guess_format(tool_options.name_for_first);
  }
  if (parser.options.count("in2")) {
    if (1 < parser.options.count("in2")) {
      std::cerr << "warning: second input format has already been specified; extra option ignored\n";
    }

    tool_options.format_for_second = lts::parse_format(parser.option_argument("in2").c_str());

    if (tool_options.format_for_second == lts_none) {
      std::cerr << "warning: format '" << parser.option_argument("in2") <<
                   "' is not recognised; option ignored" << std::endl;
    }
  }
  else {
    tool_options.format_for_second = lts::guess_format(tool_options.name_for_second);
  }

  return tool_options;
}

int process(t_tool_options const & tool_options) {
  lts l1,l2;

  if ( tool_options.name_for_first.empty() ) {
    gsVerboseMsg("reading first LTS from stdin...\n");

    if ( !l1.read_from(std::cin, tool_options.format_for_first) ) {
      throw std::runtime_error("cannot read LTS from stdin");
    }
  } else {
    gsVerboseMsg("reading first LTS from '%s'...\n", tool_options.name_for_first.c_str());

    if ( !l1.read_from(tool_options.name_for_first.c_str(), tool_options.format_for_first) ) {
      throw std::runtime_error("cannot read LTS from file '" + tool_options.name_for_first + "'");
    }
  }

  gsVerboseMsg("reading second LTS from '%s'...\n", tool_options.name_for_second.c_str());

  if ( !l2.read_from(tool_options.name_for_second.c_str(), tool_options.format_for_second) ) {
    throw std::runtime_error("cannot read LTS from file '" + tool_options.name_for_second + "'");
  }

  gsVerboseMsg("comparing LTSs...\n");

  bool result = l1.compare(l2,tool_options.equivalence,tool_options.eq_opts);

  gsMessage("LTSs are %s%s\n", ((result) ? "" : "not "),
                         equivalent_string(tool_options.equivalence));

  return (result) ? 0 : 2;
}

int main(int argc, char **argv) {
  MCRL2_ATERM_INIT(argc, argv)

  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    return process(parse_command_line(argc, argv));
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
