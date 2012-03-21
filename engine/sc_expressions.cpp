// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"
#include <boost/format.hpp>

namespace { // ANONYMOUS ====================================================

// Constant Expressions =====================================================

class const_expr_t : public expr_t
{
private:
  token_type_t result_type;

public:
  template <typename S>
  const_expr_t( S&& n, token_type_t type ) :
    expr_t( std::forward<S>( n ) ), result_type( type )
  {}

  virtual token_type_t evaluate() // override
  { return result_type; }
};

class num_const_expr_t : public const_expr_t
{
public:
  template <typename S>
  num_const_expr_t( S&& n, double constant_value ) :
    const_expr_t( std::forward<S>( n ), TOK_NUM )
  { result_num = constant_value; }
};

class str_const_expr_t : public const_expr_t
{
public:
  template <typename S1, typename S2>
  str_const_expr_t( S1&& n, S2&& constant_value ) :
    const_expr_t( std::forward<S1>( n ), TOK_STR )
  { result_str = std::forward<S2>( constant_value ); }
};

// Unary Operators ==========================================================

template <typename F>
class unary_t : public expr_t
{
protected:
  const std::unique_ptr<expr_t> input;
  F f;

public:
  template <typename String, typename Pointer, typename Function>
  unary_t( String&& n, Pointer&& i, Function&& f) :
    expr_t( std::forward<String>( n ) ),
    input( std::forward<Pointer>( i ) ),
    f( std::forward<Function>( f ) )
  { assert( input ); }

  virtual token_type_t evaluate()
  {
    if ( input -> evaluate() == TOK_NUM )
    {
      result_num = f( input -> result_num );
      return TOK_NUM;
    }

    throw error_t( ( boost::format( "Unexpected input type (%1%) for unary operator '%2%'" )
                     % input -> name() % name() ).str(), this );
  }
};

template <typename Name, typename Input, typename F>
inline unary_t<F>* make_unary( Name&& name, Input&& input, F&& f)
{ return new unary_t<F>( std::forward<Name>( name ), std::forward<Input>( input ), std::forward<F>( f ) ); }

template <typename Name, typename Input>
expr_t* select_unary( token_type_t op, Name&& name, Input&& input )
{
  switch( op )
  {
  case TOK_PLUS:
    return make_unary( std::forward<Name>( name ),
                       std::forward<Input>( input ),
                       []( double v ){ return v; } );
  case TOK_MINUS:
    return make_unary( std::forward<Name>( name ),
                       std::forward<Input>( input ),
                       []( double v ){ return -v; } );
  case TOK_NOT:
    return make_unary( std::forward<Name>( name ),
                       std::forward<Input>( input ),
                       []( double v ){ return ! v; } );
  case TOK_ABS:
    return make_unary( std::forward<Name>( name ),
                       std::forward<Input>( input ),
                       []( double v ){ return std::fabs( v ); } );
  case TOK_FLOOR:
    return make_unary( std::forward<Name>( name ),
                       std::forward<Input>( input ),
                       []( double v ){ return std::floor( v ); } );
  case TOK_CEIL:
    return make_unary( std::forward<Name>( name ),
                       std::forward<Input>( input ),
                       []( double v ){ return std::ceil( v ); } );
  default:
    // FIXME: report invalid operation code.
    assert( false );
    return nullptr;
  }
}

// Binary Operators =========================================================

class binary_t : public expr_t
{
protected:
  const std::unique_ptr<expr_t> left, right;

  static bool bool_operand( const std::unique_ptr<expr_t>& operand )
  {
    switch ( operand -> evaluate() )
    {
    case TOK_NUM:
      return operand -> result_num != 0;
    default:
      return true;
    }
  }

  void expect_left( token_type_t type )
  {
    if ( likely( left -> evaluate() == type ) ) return;

    throw error_t( ( boost::format( "Invalid input type %1% for binary operator '%2%'" )
                     % left -> name() % name() ).str(), this );
  }

  void expect_right( token_type_t type )
  {
    if ( likely( right -> evaluate() == type ) ) return;

    throw error_t( ( boost::format( "Inconsistent input types (%1% and %2%) for binary operator '%3%'" )
                     % left -> name() % right -> name() % name() ).str(), this );
  }

public:
  template <typename Name, typename Left, typename Right>
  binary_t( Name&& n, Left&& l, Right&& r ) :
    expr_t( std::forward<Name>( n ) ),
    left( std::forward<Left>( l ) ), right( std::forward<Right>( r ) )
  {
    assert( left );
    assert( right );
  }
};

template <template <typename> class F>
class binary_op_t : public binary_t
{
private:
  static double f( double left, double right )
  { return F<double>()( left, right ); }

public:
  template <typename Name, typename Left, typename Right>
  binary_op_t( Name&& n, Left&& l, Right&& r ) :
    binary_t( std::forward<Name>( n ), std::forward<Left>( l ), std::forward<Right>( r ) )
  {}

  virtual token_type_t evaluate() // override
  {
    expect_left( TOK_NUM );
    expect_right( TOK_NUM );
    result_num = f( left -> result_num, right -> result_num );
    return TOK_NUM;
  }
};

template <template <typename> class F>
class binary_compare_t : public binary_t
{
private:
  template <typename T>
  static bool f( T a, T b )
  { return F<T>()( a, b ); }

public:
  template <typename Name, typename Left, typename Right>
  binary_compare_t( Name&& n, Left&& l, Right&& r ) :
    binary_t( std::forward<Name>( n ), std::forward<Left>( l ), std::forward<Right>( r ) )
  {}

  virtual token_type_t evaluate() // override
  {
    token_type_t result_type = left -> evaluate();
    expect_right( result_type );

    switch( result_type )
    {
    case TOK_NUM:
      result_num = f( left -> result_num, right -> result_num );
      break;
    case TOK_STR:
      result_num = f( left -> result_str, right -> result_str );
      break;
    default:
      throw error_t( ( boost::format( "Invalid operand type '%1%' for binary operator '%2%'" )
                       % left -> name() %  name() ).str(), this );
    }

    return TOK_NUM;
  }
};

class logical_and_t : public binary_t
{
public:
  template <typename Name, typename Left, typename Right>
  logical_and_t( Name&& n, Left&& l, Right&& r ) :
    binary_t( std::forward<Name>( n ), std::forward<Left>( l ), std::forward<Right>( r ) )
  {}

  virtual token_type_t evaluate() // override
  {
    result_num = bool_operand( left ) && bool_operand( right );
    return TOK_NUM;
  }
};

class logical_or_t : public binary_t
{
public:
  template <typename Name, typename Left, typename Right>
  logical_or_t( Name&& n, Left&& l, Right&& r ) :
    binary_t( std::forward<Name>( n ), std::forward<Left>( l ), std::forward<Right>( r ) )
  {}

  virtual token_type_t evaluate() // override
  {
    result_num = bool_operand( left ) || bool_operand( right );
    return TOK_NUM;
  }
};

template <typename Name, typename Left, typename Right>
expr_t* select_binary( Name&& n, token_type_t operation, Left&& l, Right&& r )
{
  switch( operation )
  {
  case TOK_ADD:
    return new binary_op_t<std::plus>( std::forward<Name>( n ),
                                       std::forward<Left>( l ),
                                       std::forward<Right>( r ) );
  case TOK_SUB:
    return new binary_op_t<std::minus>( std::forward<Name>( n ),
                                        std::forward<Left>( l ),
                                        std::forward<Right>( r ) );
  case TOK_MULT:
    return new binary_op_t<std::multiplies>( std::forward<Name>( n ),
                                             std::forward<Left>( l ),
                                             std::forward<Right>( r ) );
  case TOK_DIV:
    return new binary_op_t<std::divides>( std::forward<Name>( n ),
                                          std::forward<Left>( l ),
                                          std::forward<Right>( r ) );
  case TOK_EQ:
    return new binary_compare_t<std::equal_to>( std::forward<Name>( n ),
                                                std::forward<Left>( l ),
                                                std::forward<Right>( r ) );
  case TOK_NOTEQ:
    return new binary_compare_t<std::not_equal_to>( std::forward<Name>( n ),
                                                    std::forward<Left>( l ),
                                                    std::forward<Right>( r ) );
  case TOK_LT:
    return new binary_compare_t<std::less>( std::forward<Name>( n ),
                                            std::forward<Left>( l ),
                                            std::forward<Right>( r ) );
  case TOK_LTEQ:
    return new binary_compare_t<std::less_equal>( std::forward<Name>( n ),
                                                  std::forward<Left>( l ),
                                                  std::forward<Right>( r ) );
  case TOK_GT:
    return new binary_compare_t<std::greater>( std::forward<Name>( n ),
                                               std::forward<Left>( l ),
                                               std::forward<Right>( r ) );
  case TOK_GTEQ:
    return new binary_compare_t<std::greater_equal>( std::forward<Name>( n ),
                                                     std::forward<Left>( l ),
                                                     std::forward<Right>( r ) );
  case TOK_AND:
    return new logical_and_t( std::forward<Name>( n ),
                              std::forward<Left>( l ),
                              std::forward<Right>( r ));
  case TOK_OR:
    return new logical_or_t( std::forward<Name>( n ),
                             std::forward<Left>( l ),
                             std::forward<Right>( r ));
  default:
    assert( false );
    return nullptr;
  }
}

} // ANONYMOUS namespace ====================================================

// precedence ===============================================================

int expression_t::precedence( token_type_t expr_token_type )
{
  switch ( expr_token_type )
  {
  case TOK_FLOOR:
  case TOK_CEIL:
    return 6;

  case TOK_NOT:
  case TOK_PLUS:
  case TOK_MINUS:
  case TOK_ABS:
    return 5;

  case TOK_MULT:
  case TOK_DIV:
    return 4;

  case TOK_ADD:
  case TOK_SUB:
    return 3;

  case TOK_EQ:
  case TOK_NOTEQ:
  case TOK_LT:
  case TOK_LTEQ:
  case TOK_GT:
  case TOK_GTEQ:
  case TOK_IN:
  case TOK_NOTIN:
    return 2;

  case TOK_AND:
  case TOK_OR:
    return 1;

  default:
    assert( 0 );
    return 0;
  }
}

// is_unary =================================================================

bool expression_t::is_unary( token_type_t expr_token_type )
{
  switch ( expr_token_type )
  {
  case TOK_NOT:
  case TOK_PLUS:
  case TOK_MINUS:
  case TOK_ABS:
  case TOK_FLOOR:
  case TOK_CEIL:
    return true;
  default:
    return false;
  }
}

// is_binary ================================================================

bool expression_t::is_binary( token_type_t expr_token_type )
{
  switch ( expr_token_type )
  {
  case TOK_MULT:
  case TOK_DIV:
  case TOK_ADD:
  case TOK_SUB:
  case TOK_EQ:
  case TOK_NOTEQ:
  case TOK_LT:
  case TOK_LTEQ:
  case TOK_GT:
  case TOK_GTEQ:
  case TOK_AND:
  case TOK_OR:
  case TOK_IN:
  case TOK_NOTIN:
    return true;
  default:
    return false;
  }
}

// next_token ===============================================================

token_type_t expression_t::next_token( action_t* action,
                                       const std::string& expr_str,
                                       int& current_index,
                                       std::string& token_str,
                                       token_type_t prev_token )
{
  unsigned char c = expr_str[ current_index++ ];

  if ( c == '\0' ) return TOK_UNKNOWN;
  if ( ( prev_token == TOK_FLOOR || prev_token == TOK_CEIL ) && c != '(' )
    return TOK_UNKNOWN;

  token_str = c;

  if ( c == '@' ) return TOK_ABS;
  if ( c == '+' ) return TOK_ADD;
  if ( c == '-' && ( prev_token == TOK_STR || prev_token == TOK_NUM ) ) return TOK_SUB;
  if ( c == '-' && prev_token != TOK_STR && prev_token != TOK_NUM && ! isdigit( expr_str[ current_index ] ) ) return TOK_SUB;
  if ( c == '*' ) return TOK_MULT;
  if ( c == '%' ) return TOK_DIV;
  if ( c == '&' )
  {
    if ( expr_str[ current_index ] == '&' ) current_index++;
    return TOK_AND;
  }
  if ( c == '|' )
  {
    if ( expr_str[ current_index ] == '|' ) current_index++;
    return TOK_OR;
  }
  if ( c == '~' )
  {
    if ( expr_str[ current_index ] == '~' ) current_index++;
    return TOK_IN;
  }
  if ( c == '!' )
  {
    if ( expr_str[ current_index ] == '=' ) { current_index++; token_str += "="; return TOK_NOTEQ; }
    if ( expr_str[ current_index ] == '~' ) { current_index++; token_str += "~"; return TOK_NOTIN; }
    return TOK_NOT;
  }
  if ( c == '=' )
  {
    if ( expr_str[ current_index ] == '=' ) current_index++;
    return TOK_EQ;
  }
  if ( c == '<' )
  {
    if ( expr_str[ current_index ] == '=' ) { current_index++; token_str += "="; return TOK_LTEQ; }
    return TOK_LT;
  }
  if ( c == '>' )
  {
    if ( expr_str[ current_index ] == '=' ) { current_index++; token_str += "="; return TOK_GTEQ; }
    return TOK_GT;
  }
  if ( c == '(' ) return TOK_LPAR;
  if ( c == ')' ) return TOK_RPAR;

  if ( isalpha( c ) )
  {
    c = expr_str[ current_index ];
    while ( isalpha( c ) || isdigit( c ) || c == '_' || c == '.' )
    {
      token_str += c;
      c = expr_str[ ++current_index ];
    }

    if ( util_t::str_compare_ci( token_str, "floor" ) )
      return TOK_FLOOR;
    else if ( util_t::str_compare_ci( token_str, "ceil" ) )
      return TOK_CEIL;
    else
      return TOK_STR;
  }

  if ( isdigit( c ) || c == '-' )
  {
    c = expr_str[ current_index ];
    while ( isdigit( c ) || c == '.' )
    {
      token_str += c;
      c = expr_str[ ++current_index ];
    }

    return TOK_NUM;
  }

  if ( action )
  {
    action -> sim -> errorf( "%s-%s: Unexpected token (%c) in %s\n", action -> player -> name(), action -> name(), c, expr_str.c_str() );
  }
  else
  {
    printf( "Unexpected token (%c) in %s\n", c, expr_str.c_str() );
  }

  return TOK_UNKNOWN;
}

// parse_tokens =============================================================

std::vector<expr_token_t>
expression_t::parse_tokens( action_t* action,
                            const std::string& expr_str )
{
  std::vector<expr_token_t> tokens;
  expr_token_t token;
  int current_index=0;
  token_type_t t = TOK_UNKNOWN;

  while ( ( token.type = next_token( action, expr_str, current_index, token.label, t ) ) != TOK_UNKNOWN )
  {
    t = token.type;
    tokens.push_back( token );
  }

  return tokens;
}

// print_tokens =============================================================

void expression_t::print_tokens( const std::vector<expr_token_t>& tokens, sim_t* sim )
{
  log_t::output( sim, "tokens:\n" );
  for ( auto& t : tokens )
    log_t::output( sim,  "%2d  '%s'\n", t.type, t.label.c_str() );
}

// convert_to_unary =========================================================

void expression_t::convert_to_unary( std::vector<expr_token_t>& tokens )
{
  size_t num_tokens = tokens.size();
  for ( size_t i=0; i < num_tokens; i++ )
  {
    expr_token_t& t = tokens[ i ];

    bool left_side = false;

    if ( i > 0 )
      if ( tokens[ i-1 ].type == TOK_NUM ||
           tokens[ i-1 ].type == TOK_STR ||
           tokens[ i-1 ].type == TOK_RPAR )
        left_side = true;

    if ( ! left_side && ( t.type == TOK_ADD || t.type == TOK_SUB ) )
    {
      // Must be unary.
      t.type = ( t.type == TOK_ADD ) ? TOK_PLUS : TOK_MINUS;
    }
  }
}

// convert_to_rpn ===========================================================

bool expression_t::convert_to_rpn( std::vector<expr_token_t>& tokens )
{
  std::vector<expr_token_t> rpn, stack;

  size_t num_tokens = tokens.size();
  for ( size_t i=0; i < num_tokens; i++ )
  {
    expr_token_t& t = tokens[ i ];

    if ( t.type == TOK_NUM || t.type == TOK_STR )
    {
      rpn.push_back( t );
    }
    else if ( t.type == TOK_LPAR )
    {
      stack.push_back( t );
    }
    else if ( t.type == TOK_RPAR )
    {
      while ( true )
      {
        if ( stack.empty() ) { printf( "rpar stack empty\n" ); return false; }
        expr_token_t& s = stack.back();
        if ( s.type == TOK_LPAR )
        {
          stack.pop_back();
          break;
        }
        else
        {
          rpn.push_back( s );
          stack.pop_back();
        }
      }
    }
    else // operator
    {
      while ( true )
      {
        if ( stack.empty() ) break;
        expr_token_t& s = stack.back();
        if ( s.type == TOK_LPAR ) break;
        if ( precedence( t.type ) > precedence( s.type ) ) break;
        rpn.push_back( s );
        stack.pop_back();
      }
      stack.push_back( t );
    }
  }

  while ( ! stack.empty() )
  {
    expr_token_t& s = stack.back();
    if ( s.type == TOK_LPAR ) { printf( "stack lpar\n" ); return false; }
    rpn.push_back( s );
    stack.pop_back();
  }

  tokens.swap( rpn );

  return true;
}

// build_expression_tree ====================================================

static expr_t* build_expression_tree( action_t* action,
                                      std::vector<expr_token_t>& tokens )
{
  std::vector<std::unique_ptr<expr_t>> stack;

  for ( expr_token_t& t : tokens )
  {
    if ( t.type == TOK_NUM )
      stack.emplace_back( new num_const_expr_t( t.label, atof( t.label.c_str() ) ) );

    else if ( t.type == TOK_STR )
    {
      std::unique_ptr<expr_t> e( action -> create_expression( t.label ) );
      if ( ! e )
      {
        action -> sim -> errorf( "Player %s action %s : Unable to decode expression function '%s'\n",
                                 action -> player -> name(), action -> name(), t.label.c_str() );
        return nullptr;
      }
      stack.emplace_back( std::move( e ) );
    }

    else if ( expression_t::is_unary( t.type ) )
    {
      if ( stack.size() < 1 )
        return nullptr;
      std::unique_ptr<expr_t> input = std::move( stack.back() ); stack.pop_back();
      assert( input );
      stack.emplace_back( select_unary( t.type, t.label, std::move( input ) ) );
    }

    else if ( expression_t::is_binary( t.type ) )
    {
      if ( stack.size() < 2 )
        return nullptr;
      std::unique_ptr<expr_t> right = std::move( stack.back() ); stack.pop_back();
      std::unique_ptr<expr_t> left  = std::move( stack.back() ); stack.pop_back();
      if ( ! left || ! right )
        return nullptr;
      stack.emplace_back( select_binary( t.label, t.type,
                                         std::move( left ), std::move( right ) ) );
    }

    else
      assert( false );
  }

  if ( stack.size() != 1 )
    return nullptr;

  return stack.back().release();
}

// expr_t::parse ============================================================

expr_t* expr_t::parse( action_t* action, const std::string& expr_str )
{
  if ( expr_str.empty() ) return 0;

  std::vector<expr_token_t> tokens = expression_t::parse_tokens( action, expr_str );

  if ( action -> sim -> debug ) expression_t::print_tokens( tokens, action -> sim );

  expression_t::convert_to_unary( tokens );

  if ( action -> sim -> debug ) expression_t::print_tokens( tokens, action -> sim );

  if ( ! expression_t::convert_to_rpn( tokens ) )
  {
    action -> sim -> errorf( "%s-%s: Unable to convert %s into RPN\n", action -> player -> name(), action -> name(), expr_str.c_str() );
    action -> sim -> cancel();
    return 0;
  }

  if ( action -> sim -> debug ) expression_t::print_tokens( tokens, action -> sim );

  expr_t* e = build_expression_tree( action, tokens );

  if ( ! e )
  {
    action -> sim -> errorf( "%s-%s: Unable to build expression tree from %s\n", action -> player -> name(), action -> name(), expr_str.c_str() );
    action -> sim -> cancel();
    return 0;
  }

  return e;
}

#ifdef UNIT_TEST
// FIXME

int main( int argc, char** argv )
{
  for ( int i=1; i < argc; i++ )
  {
    std::vector<expr_token_t> tokens;

    parse_tokens( NULL, tokens, argv[ i ] );
    convert_to_unary( NULL, tokens );
    print_tokens( tokens );

    if ( convert_to_rpn( NULL, tokens ) )
    {
      printf( "rpn:\n" );
      print_tokens( tokens );

      action_expr_t* expr = build_expression_tree( 0, tokens );

      if ( expr )
      {
        printf( "evaluate:\n" );
        std::string buffer;
        expr -> evaluate();
        if ( expr -> result_type == TOK_NUM )
        {
          printf( "%f\n", expr -> result_num );
        }
        else if ( expr -> result_type == TOK_STR )
        {
          printf( "%s\n", expr -> result_str.c_str() );
        }
        else
        {
          printf( "unknown\n" );
        }
      }
    }
  }

  return 0;
}

#endif
