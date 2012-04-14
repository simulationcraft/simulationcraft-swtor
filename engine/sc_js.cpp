// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

namespace js { // ===========================================================

namespace { // ANONYMOUS NAMESPACE ==========================================

// is_white_space ===========================================================

inline bool is_white_space( char c )
{ return std::isspace( c ); }

// is_token_char ============================================================

inline bool is_token_char( char c )
{
  switch( c )
  {
  case '-':
  case '+':
  case '_':
  case '.':
  case '$':
    return true;
  default:
    return std::isalpha( c ) || std::isdigit( c );
  }
}

// parse_token ==============================================================

char parse_token( std::string&            token_str,
                  const std::string&      input,
                  std::string::size_type& index )
{
  token_str.clear();

  while ( is_white_space( input[ index ] ) ) index++;

  char c = input[ index++ ];

  if ( c == '{' || c == '}' ||
       c == '[' || c == ']' ||
       c == '(' || c == ')' ||
       c == ',' || c == ':' ||
       c == '=' )
  {
    return c;
  }

  if ( c == '\'' || c == '\"' )
  {
    char start = c;
    while ( true )
    {
      c = input[ index++ ];
      if ( c == '\0' ) return c;
      if ( c == start ) break;
      if ( c == '\\' )
      {
        c = input[ index++ ];
        switch ( c )
        {
        case 'n': c = '\n'; break;
        case 'r': c = '\r'; break;
        case 't': c = '\t'; break;
        case '\0': return c;
        }
      }
      token_str += c;
    }
    return 'S';
  }
  else if ( is_token_char( c ) )
  {
    token_str += c;
    while ( is_token_char( input[ index ] ) ) token_str += input[ index++ ];
    if ( token_str == "new" ) return 'N';
    if ( token_str == "null" ) return 'U';
    return 'S';
  }

  return '\0';
}

// parse_value ==============================================================

void parse_value( sim_t*                  sim,
                  node_t*                 node,
                  char                    token_type,
                  std::string&            token_str,
                  const std::string&      input,
                  std::string::size_type& index )
{
  if ( token_type == '{' )
  {
    while ( ! sim -> canceled )
    {
      token_type = parse_token( token_str, input, index );
      if ( token_type == '}' ) break;
      if ( token_type != 'S' )
      {
        sim -> errorf( "Unexpected token '%c' (%s) at index %d (%s)\n",
                       token_type, token_str.c_str(), ( int ) index, node -> name().c_str() );
        sim -> cancel();
        return;
      }
      node_t* child = new node_t( token_str );
      node -> add_child( child );

      token_type = parse_token( token_str, input, index );
      if ( token_type != ':' )
      {
        sim -> errorf( "Unexpected token '%c' at index %d (%s)\n",
                       token_type, ( int ) index, node -> name().c_str() );
        sim -> cancel();
        return;
      }

      token_type = parse_token( token_str, input, index );

      // Apparently Blizzard does not know what kind of pets Hunters have in their game
      if ( token_type == ',' ) continue;

      parse_value( sim, child, token_type, token_str, input, index );

      token_type = parse_token( token_str, input, index );
      if ( token_type == ',' ) continue;
      if ( token_type == '}' ) break;

      sim -> errorf( "Unexpected token '%c' at index %d (%s)\n",
                     token_type, ( int ) index, node -> name().c_str() );
      sim -> cancel();
      return;
    }
  }
  else if ( token_type == '[' )
  {
    int  array_index=-1;

    while ( ! sim -> canceled )
    {
      token_type = parse_token( token_str, input, index );
      if ( token_type == ']' ) break;

      node_t* child = new node_t( ( boost::format( "%d" ) % ++array_index ).str() );
      node -> add_child( child );

      parse_value( sim, child, token_type, token_str, input, index );

      token_type = parse_token( token_str, input, index );
      if ( token_type == ',' ) continue;
      if ( token_type == ']' ) break;

      sim -> errorf( "Unexpected token '%c' at index %d (%s)\n",
                     token_type, ( int ) index, node -> name().c_str() );
      sim -> cancel();
      return;
    }
  }
  else if ( token_type == 'N' )
  {
    token_type = parse_token( token_str, input, index );
    if ( token_type != 'S' )
    {
      sim -> errorf( "Unexpected token '%c' at index %d (%s)\n",
                     token_type, ( int ) index, node -> name().c_str() );
      sim -> cancel();
      return;
    }
    node -> value( token_str );

    token_type = parse_token( token_str, input, index );
    if ( token_type != '(' )
    {
      sim -> errorf( "Unexpected token '%c' at index %d (%s)\n",
                     token_type, ( int ) index, node -> name().c_str() );
      sim -> cancel();
      return;
    }

    std::string::size_type start = index;
    token_type = parse_token( token_str, input, index );
    if ( token_type == ')' ) return;
    index = start;

    while ( ! sim -> canceled )
    {
      node_t* child = new node_t( node -> value() );
      node -> add_child( child );

      token_type = parse_token( token_str, input, index );
      parse_value( sim, child, token_type, token_str, input, index );

      token_type = parse_token( token_str, input, index );
      if ( token_type == ',' ) continue;
      if ( token_type == ')' ) break;

      sim -> errorf( "Unexpected token '%c' at index %d (%s)\n"
                     , token_type, ( int ) index, node -> name().c_str() );
      sim -> cancel();
      return;
    }
  }
  else if ( token_type == 'S' )
  {
    node -> value( token_str );

    std::string::size_type start = index;
    token_type = parse_token( token_str, input, index );

    if ( token_type == '=' )
    {
      token_type = parse_token( token_str, input, index );
      node_t* child = new node_t( node -> value() );
      node -> add_child( child );
      parse_value( sim, child, token_type, token_str, input, index );
    }
    else
      index = start;
  }
  else if ( token_type == 'U' )
  {
    ; // Nothing to do.
  }
  else
  {
    sim -> errorf( "Unexpected token '%c' at index %d (%s)\n"
                   , token_type, ( int ) index, node -> name().c_str() );
    sim -> cancel();
  }
}

// split_path ===============================================================

const node_t* split_path( const node_t*      node,
                          const std::string& path )
{
  for ( auto& component : split( path, '/' ) )
  {
    node = node -> get_child( component );
    if ( ! node ) return nullptr;
  }

  return node;
}

inline node_t* split_path( node_t*            node,
                           const std::string& path )
{
  return const_cast<node_t*>( split_path( const_cast<const node_t*>( node ), path ) );
}

} // ANONYMOUS NAMESPACE ====================================================

node_t::~node_t() {}

bool node_t::get( int& out ) const
{
  if ( value_.empty() ) return false;
  out = atoi( value_.c_str() );
  return true;
}

bool node_t::get( const std::string& path, int& out ) const
{
  if ( const node_t* node = split_path( this, path ) )
    return node -> get( out );
  else
    return false;
}

bool node_t::get( double& out ) const
{
  if ( value_.empty() ) return false;
  out = atof( value_.c_str() );
  return true;
}

bool node_t::get( const std::string& path, double& out ) const
{
  if ( const node_t* node = split_path( this, path ) )
    return node -> get( out );
  else
    return false;
}

bool node_t::get( std::string& out ) const
{
  if ( value_.empty() ) return false;
  out = value_;
  return true;
}

bool node_t::get( const std::string& path, std::string& out ) const
{
  if ( const node_t* node = split_path( this, path ) )
    return node -> get( out );
  else
    return false;
}

// node_t::get_child ========================================================

node_t* node_t::get_child( const std::string& name ) const
{
  for ( auto const& node : *this )
    if ( name == node -> name() )
      return get_pointer( node );

  return nullptr;
}

// node_t::find =============================================================

const node_t* node_t::find( const std::string& path ) const
{
  if ( path.empty() || path == name_ )
    return this;

  return split_path( this, path );
}

// node_t::print ============================================================

void node_t::print( FILE* file, int spacing ) const
{
  if ( ! file ) file = stdout;

  util_t::fprintf( file, "%*s%s", spacing, "", name_.c_str() );

  if ( ! value_.empty() )
    util_t::fprintf( file, " : '%s'", value_.c_str() );
  util_t::fprintf( file, "\n" );

  for ( auto const& child : children_ )
    child -> print( file, spacing + 2 );
}

// node_t::get_child_values =================================================

std::vector<std::string> node_t::get_child_values() const
{
  std::vector<std::string> values;
  values.reserve( children_.size() );
  for( auto& child : *this )
    values.push_back( child -> value() );
  return values;
}

// js::create ===============================================================

handle create( sim_t* sim, const std::string& input )
{
  if ( input.empty() )
    return handle( nullptr );

  handle root( new node_t( "root" ) );

  std::string::size_type index=0;

  std::string token_str;
  char token_type = parse_token( token_str, input, index );

  parse_value( sim, root, token_type, token_str, input, index );

  return root;
}

// js::create ===============================================================

handle create( sim_t* sim, FILE* input )
{
  if ( ! input ) return handle( nullptr );

  std::string buffer;
  char c;
  while ( ( c = fgetc( input ) ) != EOF ) buffer += c;
  return create( sim, buffer );
}

} // namespace js ===========================================================
