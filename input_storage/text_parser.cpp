#include "boost/algorithm/string/trim.hpp"
#include "input_storage.h"
#include "exception_trapping.h"
#include "ApplicationTextReader.h"
#include "ItemInputState.h"
#include "IncludeFileState.h"
#define FCALL extern "C"



// These functions must be provided by the client application for the library to link properly
// They are normally generated by a generate.py script automatically
ApplicationTextReader::InputStateMap input_state_map();
const std::vector<string> profile(const string & name);

///////////////////
FCALL void set_active_profile_f(const char* profile_name, 
                                int* ierror, 
                                int profilelen)
{
_TRAP_EXCEPT(*ierror,
  ApplicationTextReader & reader = ApplicationTextReader::instance();
  string name(profile_name,profilelen);
  if (name == "all")
  {
      reader.setActiveItems(reader.allKeywords());
  }
  else
  {
      reader.setActiveItems(profile(name));
  }
) // end exception trap  
}

FCALL void set_initial_context_profile_f(const char* profile_name, int* ierror, int profilelen)
{
_TRAP_EXCEPT(*ierror,
  ApplicationTextReader & reader = ApplicationTextReader::instance();
  string name(profile_name,profilelen);
  if (name == "all")
  {
      reader.setInitialContextItems(reader.allKeywords());
  }
  else
  {
      reader.setInitialContextItems(profile(name));
  }
) // end exception trap  

}
/////////////////

FCALL void set_user_substitution_enabled_f(const bool * enabled, int* ierror)
{
_TRAP_EXCEPT(*ierror,
    ApplicationTextReader & reader = ApplicationTextReader::instance();   
    reader.getTextSubstitution().setEnabledUser(*enabled);
) // end exception trap    
}

/////////////////

FCALL void set_os_env_substitution_enabled_f(const bool * enabled, int* ierror)
{
_TRAP_EXCEPT(*ierror,
    ApplicationTextReader & reader = ApplicationTextReader::instance();   
    reader.getTextSubstitution().setEnabledEnvironment(*enabled);
) // end exception trap    
}

/////////////////

FCALL void set_substitution_not_found_is_error_f(const bool * is_error, int* ierror)
{
_TRAP_EXCEPT(*ierror,
    ApplicationTextReader & reader = ApplicationTextReader::instance();   
    reader.getTextSubstitution().setNotFoundIsError(*is_error);
) // end exception trap    
}

////////////////////
FCALL void process_text_substitution_f(int* ierror)
{
_TRAP_EXCEPT(*ierror,
    ApplicationTextReader & reader = ApplicationTextReader::instance();
    EnvSubstitution sub;
    vector<envvar> &envvars = HDFTableManager<envvar>::instance().buffer();
    for ( size_t i = 0 ; i < envvars.size();i++)
      {
	sub.add(envvars[i].name, envvars[i].value);
      }
    reader.setTextSubstitution(sub);
) // end exception trap    
}

//////////////////////
FCALL void init_file_reader_f(int* ierror)
{
_TRAP_EXCEPT(*ierror,
  //ApplicationTextReader::InputStateMap states = input_state_map();
  ApplicationTextReader & reader = ApplicationTextReader::instance();
  reader.setInputStateMap(input_state_map());
  reader.getTextSubstitution().setEnabledEnvironment(true);
  reader.getTextSubstitution().setEnabledUser(true);
  reader.setInitialContextItems(reader.allKeywords());
) // end exception trap
}

/////////////////////
FCALL void read_buffer_from_text_f(const char* startfile, int* ierror, int startfilelen)
{   
_TRAP_EXCEPT(*ierror,
    ApplicationTextReader & reader = ApplicationTextReader::instance();
    assert(reader.getInitialContextItems().size() != 0);
    assert(reader.getActiveItems().size() != 0);
    string filename(startfile,startfilelen);
    boost::algorithm::trim(filename);
    string layername=LayerManager::instance().generateLayerName(filename);  // todo: separately??
    LayerManager::instance().addLayer(layername); // todo: separately??
    reader.processInput(filename);
) // end exception trap
}



