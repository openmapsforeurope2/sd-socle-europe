#include <ign/geometry/Geometry.h>

#include <limits>
#include <sstream>

#include <exception>

#include <ign/common.h>

#include <ign/transform/Transform.h>

#include <ign/geometry.h>
#include <ign/geometry/algorithm/CentroideOp.h>

#ifdef IGN_WITH_GEOS

// avoid warning "The GEOS C++ API is unstable, please use the C API instead"
#define USE_UNSTABLE_GEOS_CPP_API

#include <ign/geometry/algorithm/BufferOpGeos.h>
#include <ign/geometry/algorithm/BooleanOpGeos.h>
#include <ign/geometry/algorithm/DistanceOpGeos.h>
#include <ign/geometry/algorithm/ConvexHullOpGeos.h>
#include <ign/geometry/algorithm/GeometryQueryOpGeos.h>
#include <ign/geometry/algorithm/GeometryManipOpGeos.h>
#include <ign/geometry/algorithm/BoundaryOpGeos.h>
#include <ign/geometry/algorithm/CentroideOpGeos.h>
#include <ign/geometry/algorithm/InteriorPointOpGeos.h>
#include <ign/geometry/algorithm/NormalizeOpGeos.h>
#endif

#include <ign/geometry/detail/CollectPointVisitor.h>
#include <ign/geometry/detail/AssignMetadataVisitor.h>
#include <ign/geometry/detail/PointVisitorFillZ.h>
#include <ign/geometry/detail/ApplyTransformVisitor.h>

#include <ign/geometry/io/WktWriter.h>
#include <ign/geometry/io/GeoJsonWriter.h>

using namespace ign ;
using namespace ign::geometry ;

std::atomic<int> Geometry::liveCount{0};

///
///
///
Geometry::Geometry():
	_geometryMetadata(0)
{
	++liveCount;
}

///
///
///
Geometry::Geometry( Geometry const& other ):
		_geometryMetadata( other._geometryMetadata)
{
	++liveCount;
}

///
///
///
Geometry::~Geometry()
{
	--liveCount;
}

///
///
///
PrecisionModel const & Geometry::getPrecisionModel() const
{
	return PrecisionModel::GetDefaultPrecisionModel();
}

///
///
///
std::string		Geometry::getGeometryTypeName() const
{
	return Geometry::GeometryTypeName(getGeometryType());
}

///
///
///
std::string	 Geometry::GeometryTypeName( GeometryType const& geometryType )
{
	switch(geometryType){
		case Geometry::GeometryTypeNull:
			return "GEOMETRY";
		case Geometry::GeometryTypePoint:
			return Point::GeometryType();
		case Geometry::GeometryTypeLineString:
			return LineString::GeometryType();
		case Geometry::GeometryTypePolygon:
			return ign::geometry::Polygon::GeometryType();
		case Geometry::GeometryTypeTriangle:
			return Triangle::GeometryType();
		case Geometry::GeometryTypeMultiPoint:
			return MultiPoint::GeometryType();
		case Geometry::GeometryTypeMultiLineString:
			return MultiLineString::GeometryType();
		case Geometry::GeometryTypeMultiPolygon:
			return MultiPolygon::GeometryType();
		case Geometry::GeometryTypeGeometryCollection:
			return GeometryCollection::GeometryType();
		case Geometry::GeometryTypeTriangulatedSurface:
			return TriangulatedSurface::GeometryType();
		case Geometry::GeometryTypePolyhedralSurface:
			return PolyhedralSurface::GeometryType();
	}
	IGN_THROW_EXCEPTION("[FATAL]Geometry::GeometryType(GeometryType const& geometryType)");
}


///
///
///
Geometry::CoordinateType Geometry::getCoordinateType() const
{
	if ( is3D() ){
		if ( isMeasured() )
			return CoordinateTypeXYZM;
		else
			return CoordinateTypeXYZ;
	}else{
		if ( isMeasured() )
			return CoordinateTypeXYM;
		else
			return CoordinateTypeXY;
	}
}

///
///
///
std::string 	Geometry::getCoordinateTypeNames() const
{
	switch ( getCoordinateType() ){
		case CoordinateTypeUndefined:
			return "";
		case CoordinateTypeXY:
			return "xy";
		case CoordinateTypeXYZ:
			return "xyz";
		case CoordinateTypeXYM:
			return "xym";
		case CoordinateTypeXYZM:
			return "xyzm";
	}
	//should never append
	IGN_THROW_EXCEPTION("not handled case in Geometry::getCoordinateTypeNames()");
}


///
///
///
int Geometry::dimension () const
{
	return 2 + ( is3D() ? 1 : 0 ) + ( isMeasured() ? 1 : 0 ) ;
}

///
///
///
int const & Geometry::SRID () const
{
	return getGeometryMetadata().getSRID();
}

///
///
///
void	Geometry::clearZ()
{
	detail::PointVisitorFillZ pointVisitorFillZ( numeric::Numeric< double >::NaN() );
	accept( pointVisitorFillZ );
}

///
///
///
void	Geometry::setFillZ( double const& zValue )
{
	detail::PointVisitorFillZ pointVisitorFillZ( zValue );
	accept( pointVisitorFillZ );
}

///
///
///
Geometry* 	Geometry::toMulti() const
{
	return clone();
}

///
///
///
bool	Geometry::isPoint() const
{
	return ( Geometry::GeometryTypePoint == getGeometryType() );
}

///
///
///
bool	Geometry::isLineString() const
{
	return ( Geometry::GeometryTypeLineString == getGeometryType() );
}

///
///
///
bool	Geometry::isPolygon() const
{
	return ( Geometry::GeometryTypePolygon == getGeometryType() );
}

///
///
///
bool	Geometry::isTriangle() const
{
	return ( Geometry::GeometryTypeTriangle == getGeometryType() );
}

///
///
///
bool	Geometry::isTriangulatedSurface() const
{
	return ( Geometry::GeometryTypeTriangulatedSurface == getGeometryType() );
}

///
///
///
bool	Geometry::isPolyhedralSurface() const
{
	return ( Geometry::GeometryTypePolyhedralSurface == getGeometryType() );
}

///
///
///
bool	Geometry::isMultiPoint() const
{
	return ( Geometry::GeometryTypeMultiPoint == getGeometryType() );
}


///
///
///
bool	Geometry::isMultiLineString() const
{
	return ( Geometry::GeometryTypeMultiLineString == getGeometryType() );
}

///
///
///
bool	Geometry::isMultiPolygon() const
{
	return ( Geometry::GeometryTypeMultiPolygon == getGeometryType() );
}

///
///
///
bool	Geometry::isGeometryCollection() const
{
	return (
		Geometry::GeometryTypeGeometryCollection == getGeometryType()
	 || isMultiPoint()
	 || isMultiLineString()
	 || isMultiPolygon()
	) ;
}

///
///
///
Envelope		Geometry::getEnvelope() const
{
	Envelope bbox;
	envelope(bbox);
	return bbox;
}



///
///
///
Point const&	Geometry::asPoint() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), Point::GeometryType() ));
}

///
///
///
Point&			Geometry::asPoint()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), Point::GeometryType() ));
}

///
///
///
LineString const&	Geometry::asLineString() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), LineString::GeometryType() ));
}

///
///
///
LineString&		Geometry::asLineString()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), LineString::GeometryType() ));
}

///
///
///
ign::geometry::Polygon const&		Geometry::asPolygon() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), ign::geometry::Polygon::GeometryType() ));
}

///
///
///
ign::geometry::Polygon&		Geometry::asPolygon()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), ign::geometry::Polygon::GeometryType() ));
}

///
///
///
Triangle const&	Geometry::asTriangle() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), Triangle::GeometryType() ));
}
///
///
///
Triangle&	Geometry::asTriangle()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), Triangle::GeometryType() ));
}


///
///
///
MultiPoint const&	Geometry::asMultiPoint() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), MultiPoint::GeometryType() ));
}

///
///
///
MultiPoint&		Geometry::asMultiPoint()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), MultiPoint::GeometryType() ));
}

///
///
///
MultiLineString const&	   Geometry::asMultiLineString() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), MultiLineString::GeometryType() ));
}

///
///
///
MultiLineString&	Geometry::asMultiLineString()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), MultiLineString::GeometryType() ));
}

///
///
///
MultiPolygon const&	  Geometry::asMultiPolygon() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), MultiPolygon::GeometryType() ));
}

///
///
///
MultiPolygon&	Geometry::asMultiPolygon()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), MultiPolygon::GeometryType() ));
}

///
///
///
GeometryCollection const&  Geometry::asGeometryCollection() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), GeometryCollection::GeometryType() ));
}

///
///
///
GeometryCollection&		Geometry::asGeometryCollection()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), GeometryCollection::GeometryType() ));
}

///
///
///
TriangulatedSurface const&  Geometry::asTriangulatedSurface() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), TriangulatedSurface::GeometryType() ));
}

///
///
///
TriangulatedSurface&		Geometry::asTriangulatedSurface()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), TriangulatedSurface::GeometryType() ));
}

///
///
///
PolyhedralSurface const &	Geometry::asPolyhedralSurface() const
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), PolyhedralSurface::GeometryType() ));
}

///
///
///
PolyhedralSurface&	Geometry::asPolyhedralSurface()
{
	IGN_THROW_EXCEPTION( badCastExceptionMessage( getGeometryTypeName(), PolyhedralSurface::GeometryType() ));
}

///
///
///
std::string	Geometry::badCastExceptionMessage( std::string const& from, std::string const& to ) const
{
	std::ostringstream oss;
	oss << "[ERROR]mauvais cast de " << from << " en " << to << "!" << std::endl;
	return oss.str();
}


///
///
///
bool	Geometry::isValid() const
{
	return true;
}


///
///
///
void	Geometry::clearMetadata()
{
	_geometryMetadata = 0;
}


///
///
///
void Geometry::copyMetadataFrom( Geometry const& other )
{
	_geometryMetadata = other._geometryMetadata;
}


///
///
///
Geometry::CoordinateQuality	Geometry::MixQualities( std::vector< Geometry::CoordinateQuality > const& qualities )
{
	//on ne stocke jamais Exists
	CoordinateQuality result = Geometry::CoordinateQualityNone;
	bool isFirst = true;
	
	for ( std::vector< CoordinateQuality >::const_iterator it = qualities.begin(); it != qualities.end(); ++it )
	{
		// assert( result != exists )
		switch ( *it )
		{
			case Geometry::CoordinateQualityExists:
			{
				return *it;
			}
			case Geometry::CoordinateQualityNone:
			{
				// on a le meme resultat
				if ( Geometry::CoordinateQualityNone == result )
				{
					isFirst = false;
					continue;
				}
				
				// ce n'est pas le premier et le premier etait complet
				if ( ! isFirst )
					return Geometry::CoordinateQualityExists;
				
				// ici, le premier element est none
				isFirst = false;
				result  = Geometry::CoordinateQualityNone;//normalement deja affecte
			}
			case Geometry::CoordinateQualityAll:
				// si le resultat est deja "All"
				if ( Geometry::CoordinateQualityAll == result )
				{
					continue;
				}
				// si n'est pas le premier alors qu'aucune coordonnee etait remplie
				if ( ! isFirst )
					return Geometry::CoordinateQualityExists;
				
				// c'est le premier, toutes les coordonnees sont remplies
				isFirst = false;
				result  = Geometry::CoordinateQualityAll;//normalement deja affecte
		}
	}
	return result;
}


///
///
///
void	Geometry::setGeometryMetadata( GeometryMetadata * geometryMetadata, bool recursive /*= true*/ )
{
	if ( recursive ){
		detail::AssignGeometryMetadata assignMetadataVisitor(geometryMetadata);
		accept( assignMetadataVisitor );
	}else{
		_geometryMetadata = geometryMetadata;
	}
}

///
///
///
GeometryMetadata const & Geometry::getGeometryMetadata() const
{
	if ( _geometryMetadata ){
		return *_geometryMetadata;
	}else{
		return GeometryMetadata::DefaultMetadata();
	}
}

///
///
///
std::string Geometry::toString() const
{
	return asText();
}


///
///
///
data::Value::ValueType const& 	Geometry::getValueType() const
{
	static data::Value::ValueType valueType = data::Value::VtGeometry ;
	return  valueType ;
}

///
///
///
std::string const& 	Geometry::getValueTypeName() const
{
	static const std::string type = "Geometry";
	return type;
}


///
///
///
void Geometry::applyTransform( transform::Transform const & transform , bool const  direct )
{
	detail::ApplyTransformVisitor applyTransformVisitor( transform , direct );
	accept(applyTransformVisitor);
}


///
///
///
void  Geometry::_asJson( std::ostream & s ) const
{
	io::GeoJsonWriter writer;
	writer.write( s, *this );
}

///
///
///
std::string		Geometry::asText( int const& numDecimals ) const
{
	std::string wkt;
	io::WktWriter writer;
	writer.write( *this, wkt, numDecimals );
	return wkt;
}

///
///
///
Point			Geometry::getCentroid() const
{
	Point p;
	
#ifdef IGN_WITH_GEOS
	algorithm::CentroideOpGeos::ComputeCentroid2d( *this, p );
#else
	algorithm::CentroideOp::ComputeCentroid2d( *this, p );
#endif
	return p;
}

///
///
///
bool    Geometry::isSimple() const
{
#ifdef IGN_WITH_GEOS
  	return algorithm::GeometryManipOpGeos::IsSimple(*this);
#else
	return true;
#endif
}

#ifdef IGN_WITH_GEOS
///
///
///
bool	Geometry::equals( Geometry const& other ) const
{
	algorithm::GeometryQueryOpGeos algo;
	return algo.query( algorithm::GeometryQueryOpGeos::QueryEquals, *this,other);
}

///
///
///
bool	Geometry::disjoint( Geometry const& other ) const
{
	algorithm::GeometryQueryOpGeos algo;
	return algo.query( algorithm::GeometryQueryOpGeos::QueryDisjoint, *this,other);
}

///
///
///
bool	Geometry::intersects( Geometry const& other ) const
{
	algorithm::GeometryQueryOpGeos algo;
	return algo.query( algorithm::GeometryQueryOpGeos::QueryIntersects, *this, other);
}

///
///
///
bool	Geometry::touches( Geometry const& other ) const
{
	algorithm::GeometryQueryOpGeos algo;
	return algo.query( algorithm::GeometryQueryOpGeos::QueryTouches, *this,other);
}

///
///
///
bool	Geometry::crosses( Geometry const& other ) const
{
	algorithm::GeometryQueryOpGeos algo;
	return algo.query( algorithm::GeometryQueryOpGeos::QueryCrosses, *this,other);
}


///
///
///
bool	Geometry::within( Geometry const& other ) const
{
	algorithm::GeometryQueryOpGeos algo;
	return algo.query( algorithm::GeometryQueryOpGeos::QueryWithin, *this,other);
}


///
///
///
bool	Geometry::contains( Geometry const& other ) const
{
	algorithm::GeometryQueryOpGeos algo;
	return algo.query( algorithm::GeometryQueryOpGeos::QueryContains, *this,other);
}

///
///
///
bool	Geometry::overlaps( Geometry const& other ) const
{
	algorithm::GeometryQueryOpGeos algo;
	return algo.query( algorithm::GeometryQueryOpGeos::QueryOverlaps, *this,other);
}

///
///
///
bool	Geometry::relate( Geometry const& other, const std::string& intersectionPattern ) const
{
	if ( this->isEmpty() || other.isEmpty() )
		return false;

	return algorithm::GeometryManipOpGeos::Relate(*this, other, intersectionPattern);
}

///
///
///
Geometry*	Geometry::boundary() const
{
	algorithm::BoundaryOpGeos algo;
	return algo.computeBoundary( *this );
}

///
///
///
Point   	Geometry::centroid() const
{
	return getCentroid();
}

///
///
///
Point			Geometry::getInteriorPoint() const
{
	Point p;
	algorithm::InteriorPointOpGeos::ComputeInteriorPoint2d( *this, p );
	return p;
}

///
///
///
Geometry*		Geometry::buffer( double distance, size_t n ) const
{
	algorithm::BufferOpGeos bufferOp;
	return bufferOp.buffer(*this,distance,n);
}

///
///
///
Geometry*		Geometry::normalize() const
{
	algorithm::NormalizeOpGeos op;
	return op.normalize(*this);
}

///
///
///
double		Geometry::distance( Geometry const& other ) const
{
	algorithm::DistanceOpGeos distanceOp;
	return distanceOp.distance(*this,other);
}

///
///
///
bool		Geometry::isWithinDistance( Geometry const& other, double const& distance ) const
{
	algorithm::DistanceOpGeos distanceOp;
	return distanceOp.isWithinDistance(*this,other,distance);
}

///
///
///
Geometry*	Geometry::convexHull() const
{
	algorithm::ConvexHullOpGeos op;
	return op.convexHull(*this);
}

///
///
///
Geometry*	Geometry::Union( Geometry const& other ) const
{
	algorithm::BooleanOpGeos op;
	return op.computeUnion( *this, other );
}

///
///
///
Geometry*	Geometry::Intersection( Geometry const& other ) const
{
	algorithm::BooleanOpGeos op;
	return op.computeIntersection( *this, other );
}

///
///
///
Geometry*	Geometry::Difference( Geometry const& other ) const
{
	algorithm::BooleanOpGeos op;
	return op.computeDifference( *this, other );
}

///
///
///
Geometry*	Geometry::SymDifference( Geometry const& other ) const
{
	algorithm::BooleanOpGeos op;
	return op.computeSymDifference( *this, other );
}

#endif
