# include "flappy_box/model/box.hpp"

using namespace flappy_box::model;

Box::Box( const std::string& n )
: GameObject( true, n )
,           _angle( 0.0 )
,            _size( 1.0 )
,        _position( 0.0, 0.0, 0.0 )
,     _maxPosition( 1.0, 1.0, 1.0 )
,        _velocity( 0.0, 0.0, 0.0 )
,    _acceleration( 0.0, 0.0, 0.0 )
,   _externalForce( 0.0, 0.0, 0.0 )
, _rotAcceleration( 0.0 )
,     _rotVelocity( 0.0 )
{

}