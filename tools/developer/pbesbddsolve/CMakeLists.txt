if(MCRL2_ENABLE_SYLVAN)

  mcrl2_add_tool(pbesbddsolve
    SOURCES
      pbesbddsolve.cpp
    DEPENDS
      mcrl2_pbes
      mcrl2_bes
      sylvan
  )

endif()
