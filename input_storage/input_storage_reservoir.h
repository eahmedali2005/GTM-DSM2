#ifndef reservoir_STORAGE_H__
#define reservoir_STORAGE_H__
/**
WARNING: THIS FILE WAS AUTOMATICALLY GENERATED USING A SCRIPT AND A TEMPLATE  
DO NOT CHANGE THE CODE HERE. 
IF THE CODE IS INCORRECT, FIX THE TEMPLATE OR SCRIPT
IF YOU WANT TO ADD NEW ITEMS, ADD THEM TO THE SCRIPT INPUT FILE AND RUN IT AFRESH
*/ 
#define _CRT_SECURE_NO_DEPRECATE  // viz studio deprecation warnings
#include "hdf5.h"
#include "hdf5_hl.h"
#include "hdf_storage.h"
#include "HDFTableManager.h"
#include "TableDescription.h"
#include "TableItemFunctors.h"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"
#include<iostream>
#include<vector>
#include<algorithm>
#include<string.h>
#include<iostream>


using namespace std;
using namespace boost;

/** Structure representing input of type reservoir.
    This class is autogenerated by a script given
    a description of the object
    \ingroup userdata
*/
class reservoir
{
public:

  /** Data type reservoir, default constructor */  
  typedef const boost::tuple<const std::string>  identifier_type;

  reservoir() :
    area(-901.0),
    bot_elev(-901.0),
    used(true),
    layer(0)
  {
    fill_n(name,32,'\0');
  };

  /** Construct a reservoir with actual data values */
  reservoir(const  char a_name[32],const double & a_area,const double & a_bot_elev, bool a_used=true, int a_layer = 0) :
    area(a_area),
    bot_elev(a_bot_elev),
    used(a_used),
    layer(a_layer)
  {
    memcpy(name,a_name,32);  
  }
  
  /**Copy constructor) 
   */
  reservoir (const reservoir & other) :
    area(other.area),
    bot_elev(other.bot_elev),
    used(other.used),
    layer(other.layer)
  {
    memcpy(name,other.name,32);  
  }
  
  /** Identifier that distinguishes whether two entries are distinct */
  identifier_type identifier()  const
  {  
     return identifier_type( name );
  }
  
  void set_identifier(identifier_type identifier)
  {
     memcpy(name,identifier.get<0>().c_str(),32);
  }
  
  /** Parent object class name.
      If this is a child item belonging to a parent, returns
      the name of the parent class. Otherwise returns the name
      of this class.
  */
  reservoir::identifier_type parent_identifier()  const
  {
     return reservoir::identifier_type( name );
  }

  /** Return the version/layer number of the parent object */ 
  int parent_version()  const
  {
    vector<reservoir>& pbuf = HDFTableManager<reservoir>::instance().buffer();
    reservoir parent;
    parent.set_identifier(parent_identifier());
    vector<reservoir>::iterator loc = lower_bound(pbuf.begin(),
                                                pbuf.end(),
                                                parent,
                                                identifier_compare<reservoir>());
    bool found = (loc!=pbuf.end()) && loc->identifier() == parent.identifier();    
    if (found && loc->used){ return loc->layer; }
    else{ return -1; }
  }

  /** Return true if this layer of this object matches the layer of the parent object that will be use in the model.*/
  bool parent_valid()  const
  {
    return this->layer == parent_version();
  }

  /** Less-than operator based on the identifier plus (for parent objects) layer number*/  
  bool operator< (const reservoir & other) const
  {
     
     if(this->identifier() != other.identifier())
	 {
		 return this->identifier() < other.identifier();
	 }
	 // todo: make this a policy
	 bool layerOutranks = (this->layer == 0 && other.layer != 0) ||
		                  (this->layer > other.layer && other.layer != 0);
     return layerOutranks;

  }

  /** Less-than operator based on the identifier plus (for parent objects) layer number*/  
  bool operator== (const reservoir & other) const
  {
     return ((*this < other ) || (other < *this)) ? false : true;
  }
  
  /** Assignment that includes all the data plus the used and layer fields */
  reservoir& operator=(const reservoir& rhs)
  {
    strcpy(this->name,rhs.name);
    this->area=rhs.area;
    this->bot_elev=rhs.bot_elev;
    used = rhs.used;
    layer = rhs.layer;
    return *this;
  }

  /** Return the class name of this object (reservoir) */
  string objectName() const
  { 
    return "reservoir"; 
  }

  
  char name[32];
  double area;
  double bot_elev;
  /** indicator that the entry is used (true if not marked deleted by user)*/
  bool used;  
  /** layer (version number) of this entry */
  int layer;
};

typedef HDFTableManager<reservoir> reservoir_table;

hid_t string_type(size_t n);

TableDescription reservoir_table_description();

istream& operator>> (istream& stream, reservoir & obj);
ostream& operator<<(ostream & stream, const reservoir & obj);



////////// FORTRAN-LINKABLE API //////////
#define FCALL extern "C"


/**
  Clear the buffer, compatible with fortran
*/  
FCALL void reservoir_clear_buffer_f();

/** query number of records being stored in buffer */
FCALL int reservoir_buffer_size_f();


/** append to buffer, compatible with fortran, returns new size*/
FCALL void reservoir_append_to_buffer_f(const  char a_name[32],const double * a_area,const double * a_bot_elev, int * ierror, 
              const int name_len);
  
/** both makes the table and writes the contents of the buffer to it */
FCALL void reservoir_write_buffer_to_hdf5_f(const hid_t* file_id, int* ierror);

/** reads the table in from a file into the buffer*/
FCALL void reservoir_read_buffer_from_hdf5_f(const hid_t* file_id, int* ierror);

/** query size information about the table in hdf5
*/
FCALL void reservoir_number_rows_hdf5_f(const hid_t* file_id, hsize_t* nrecords, int* ierror);


/** get one row worth of information from the buffer */
FCALL void reservoir_query_from_buffer_f(size_t* row, 
                         char a_name[32],double * a_area,double * a_bot_elev, int * ierror, 
              int name_len);
/**
  prioritize buffer by layers, delete unused items and sort
  */
FCALL void reservoir_prioritize_buffer_f(int* ierror);
/**
   write buffer to the given text file. File will be appended if exists and append flag is set to true.
   otherwise the file will be created or overwritten.
 */
FCALL void reservoir_write_buffer_to_text_f(const char* file, const bool* append, int* ierror, int filelen);


#endif

