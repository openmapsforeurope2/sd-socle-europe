#include <ign/ign_config.h>
#ifdef IGN_WITH_GEOS

#include <sstream>

#include <boost/cast.hpp>


#include <ign/common.h>
#include <ign/geometry.h>
#include <ign/Exception.h>
#include <ign/geometry/io/GeosIO.h>

#ifdef GEOS_NO_UNIQUE_PTR
#include <geos/geom/CoordinateArraySequence.h>
#else
#include <geos/geom/CoordinateSequence.h>
#endif

using namespace ign::geometry;
using namespace ign::geometry::io;

///
///
///
GeosIO::GeosIO():
	_geosPrecisionModel( new geos::geom::PrecisionModel() ),
	//_geometryFactory ( new geos::geom::GeometryFactory(_geosPrecisionModel.get() ) ),
    _geometryFactory ( geos::geom::GeometryFactory::create(_geosPrecisionModel.get())),
	_bestPrecisionModel( PrecisionModel::GetDefaultPrecisionModel() )
{
	if ( _bestPrecisionModel.type() == PrecisionModel::FIXED ) {
		_geosPrecisionModel.reset( new geos::geom::PrecisionModel( _bestPrecisionModel.getScaleXY() ) ) ;
		_geometryFactory = geos::geom::GeometryFactory::create(_geosPrecisionModel.get()) ;
	}else {
		// keep FLOATING
	}
}

///
///
///
GeosIO::GeosIO( PrecisionModel const& precisionModel ):
	_geosPrecisionModel( new geos::geom::PrecisionModel() ),
    _geometryFactory ( geos::geom::GeometryFactory::create(_geosPrecisionModel.get())),
	_bestPrecisionModel( precisionModel )
{
	if ( precisionModel.type() == PrecisionModel::FIXED ){
		_geosPrecisionModel.reset( new geos::geom::PrecisionModel( precisionModel.getScaleXY() ) ) ;
        _geometryFactory = geos::geom::GeometryFactory::create(_geosPrecisionModel.get()) ;
	}else{
		// keep FLOATING
	}
}

///
///
///
GeosIO::GeosIO( GeosIO const& other ):
	_bestPrecisionModel(other._bestPrecisionModel)
{
	_geosPrecisionModel.reset( new geos::geom::PrecisionModel(*( other._geosPrecisionModel.get() ) ));
    _geometryFactory = geos::geom::GeometryFactory::create(_geosPrecisionModel.get()) ;


}

/// 
///
///
GeosIO& GeosIO::operator = ( GeosIO const& other ) {
	_geosPrecisionModel.reset( new geos::geom::PrecisionModel( *(other._geosPrecisionModel.get())) );
    _geometryFactory = geos::geom::GeometryFactory::create(_geosPrecisionModel.get()) ;
	_bestPrecisionModel  = other._bestPrecisionModel;
	return *this;
}


///
///
///
GeosIO::~GeosIO()
{

}

///
///
///
void GeosIO::destroyGeosGeometry( geos::geom::Geometry* g ) const
{
	_geometryFactory->destroyGeometry( g );
//	delete g;
}


///
/// version provisoire re-ecrite
///
/*
geos::geom::Geometry*	GeosIO::newGeosGeometry( Geometry const& geometry )
{
	if ( geometry.isEmpty() )
		return 0;

	geos::io::WKTReader reader( &_geometryFactory );
	geos::geom::Geometry* geosGeom = 0;
	try {
		geosGeom = reader.read( geometry.asText( 5 ) );
	}catch ( ... )
	{
		std::ostringstream oss;
		oss << "impossible convertir '" << geometry.asText() << "' en geometrie geos";
		IGN_THROW_EXCEPTION( oss.str() );
	}
	return geosGeom;
}
*/

///
///
///
GeosGeometryPtr	GeosIO::newGeosGeometry( Geometry const& geometry )
{
    if ( geometry.isEmpty() ) return newGeosEmptyGeometry();

    GEOS_DECLARE_NEW_GEOM_PTR(geomPtr);
	switch ( geometry.getGeometryType() ){
		case Geometry::GeometryTypeNull:
			break;
		case Geometry::GeometryTypePoint:
            return newGeosPoint( geometry.asPoint() );
		case Geometry::GeometryTypeLineString:
            return newGeosLineString( geometry.asLineString() );
		case Geometry::GeometryTypePolygon:
            return newGeosPolygon( geometry.asPolygon() );
		case Geometry::GeometryTypeTriangle:
            return newGeosPolygon( geometry.asTriangle() );
		case Geometry::GeometryTypeMultiPoint:
            return newGeosMultiPoint( geometry.asMultiPoint() );
		case Geometry::GeometryTypeMultiLineString:
            return newGeosMultiLineString( geometry.asMultiLineString() );
		case Geometry::GeometryTypeMultiPolygon:
            return newGeosMultiPolygon( geometry.asMultiPolygon() );
		case Geometry::GeometryTypeTriangulatedSurface:
            return newGeosMultiPolygon( geometry.asTriangulatedSurface() );
		case Geometry::GeometryTypeGeometryCollection:
            return newGeosGeometryCollection( geometry.asGeometryCollection() );
		case Geometry::GeometryTypePolyhedralSurface:
            return newGeosMultiPolygon( geometry.asPolyhedralSurface() );
	}
	IGN_THROW_EXCEPTION( "Cas non traite dans une conversion vers GEOS(GeosIO::newGeosGeometry)" );
}

///
///
///
GeosGeometryPtr	GeosIO::newGeosEmptyGeometry()
{
#ifdef GEOS_NO_UNIQUE_PTR
	return _geometryFactory->createEmptyGeometry().release();
#else
    return _geometryFactory->createEmptyGeometry();
#endif
}

///
///
///
void               GeosIO::geosCoordinateFromPoint( Point const& point, GeosCoordinate& coords )
{
	//mborne 18/04/2012 : le snap de force sur la grille est supprime,
	// il pose des soucis sur certaines geometries. Il convient de l'appeler manuellement.

	//odorie 27/06/2013 : le snap de force est reactive pour les modeles fixes 
	// souci autrement avec les fusions de parcelles provenant de shape
	Point pointSnap(point);
	_bestPrecisionModel.snapToGrid( pointSnap );

	coords.x = pointSnap.x();
    coords.y = pointSnap.y();
    if ( ! point.is3D() ){
        coords.z = geos::DoubleNotANumber;
    }else{
        coords.z = pointSnap.z();
    }
}


///
///
///
GeosPointPtr   GeosIO::newGeosPoint( Point const& point )
{
    GeosCoordinate coords;
    geosCoordinateFromPoint( point, coords );
    return _geometryFactory->createPoint( coords );
}


///
///
///
GeosLineStringPtr  GeosIO::newGeosLineString( LineString const& lineString )
{
#ifdef GEOS_NO_UNIQUE_PTR
    geos::geom::CoordinateArraySequence* coords = new geos::geom::CoordinateArraySequence();

    for ( size_t i = 0; i < lineString.numPoints(); ++i ){
        GeosCoordinate c;
        geosCoordinateFromPoint(lineString.pointN(i),c);
        coords->add(c);
    }
#else
    geos::geom::CoordinateArraySequence coords;

    for ( size_t i = 0; i < lineString.numPoints(); ++i ){
        GeosCoordinate c;
        geosCoordinateFromPoint(lineString.pointN(i),c);
        coords.add(c);
    }
#endif
    return _geometryFactory->createLineString(coords);
}

///
///
///
GeosLinearRingPtr	GeosIO::newGeosLinearRing( LineString const& lineString )
{
#ifdef GEOS_NO_UNIQUE_PTR
	geos::geom::CoordinateArraySequence* coords = new geos::geom::CoordinateArraySequence();
	for ( size_t i = 0; i < lineString.numPoints(); ++i ){
        GeosCoordinate c;
		geosCoordinateFromPoint(lineString.pointN(i),c);
		coords->add(c);
	}
#else
    auto coords = std::make_unique<geos::geom::CoordinateArraySequence>();
    for ( size_t i = 0; i < lineString.numPoints(); ++i ){
        GeosCoordinate c;
        geosCoordinateFromPoint(lineString.pointN(i),c);
        coords->add(c);
    }
#endif
    GeosLinearRingPtr ls (_geometryFactory->createLinearRing(std::move(coords)));
	return ls;
}

///
///
///
GeosPolygonPtr  GeosIO::newGeosPolygon( ign::geometry::Polygon const& polygon )
{
    std::vector< geos::geom::LinearRing* >* interiorsRingsGeos = new std::vector< geos::geom::LinearRing* >();

	//geos 3.3 make checks
	for ( size_t i = 0; i < polygon.numInteriorRing(); ++i )
	{
		GeosLinearRingPtr ring = newGeosLinearRing( polygon.interiorRingN(i) );
		IGN_SAFE_MODE_ASSERT( ring->getGeometryTypeId() == geos::geom::GEOS_LINEARRING );
		interiorsRingsGeos->push_back( GEOS_RAW_PTR_RELEASE(ring) );
	}
    
    GeosLinearRingPtr    exteriorRingGeos (newGeosLinearRing(polygon.exteriorRing()));

    GeosPolygonPtr pg (_geometryFactory->createPolygon(*GEOS_RAW_PTR_RELEASE(exteriorRingGeos),*interiorsRingsGeos));

    return pg;
}

///
///
///
GeosPolygonPtr    GeosIO::newGeosPolygon( Triangle const& g )
{
//
#ifdef GEOS_NO_UNIQUE_PTR
	//triangle coordinates
	geos::geom::CoordinateArraySequence* coords = new geos::geom::CoordinateArraySequence();
	for ( size_t i = 0; i < 4; ++i ){
        GeosCoordinate c;
		geosCoordinateFromPoint(g[i%3],c);
		coords->add(c);
	}
	//coords->add((*coords)[0]);		// Close contour
    ign::unique_ptr<geos::geom::LinearRing> exteriorRingGeos (_geometryFactory->createLinearRing(coords));

#else
    //triangle coordinates
    geos::geom::CoordinateArraySequence coords;
    for ( size_t i = 0; i < 4; ++i ){
        GeosCoordinate c;
        geosCoordinateFromPoint(g[i%3],c);
        coords.add(c);
    }
    ign::unique_ptr<geos::geom::LinearRing> exteriorRingGeos (_geometryFactory->createLinearRing(coords));

#endif

    GeosPolygonPtr pg (_geometryFactory->createPolygon(std::move(exteriorRingGeos)).release());
    
    return pg;
}

///
///
///
GeosMultiPointPtr		GeosIO::newGeosMultiPoint( MultiPoint const& multi )
{
    std::vector<const geos::geom::Geometry*> newGeoms;
	for ( size_t i = 0; i < multi.numGeometries(); ++i ){
        newGeoms.push_back(  GEOS_RAW_PTR_RELEASE(newGeosPoint( multi.pointN(i))));
	}
    GeosMultiPointPtr  mp(_geometryFactory->createMultiPoint( newGeoms ));
    return mp;
}

///
///
///
GeosMultiLineStringPtr     GeosIO::newGeosMultiLineString( MultiLineString const& multi )
{
#ifdef GEOS_NO_UNIQUE_PTR
	std::vector< GeosGeometryPtr >* newGeoms = new std::vector< geos::geom::Geometry* >();
	for ( size_t i = 0; i < multi.numGeometries(); ++i ){
		newGeoms->push_back( newGeosLineString( multi.lineStringN(i) ) );
	}
#else
    std::vector< GeosGeometryPtr > newGeoms;
    for ( size_t i = 0; i < multi.numGeometries(); ++i ){
        newGeoms.push_back( newGeosLineString( multi.lineStringN(i) ) );
    }
#endif
    GeosMultiLineStringPtr mls(_geometryFactory->createMultiLineString( std::move(newGeoms) ));
    return mls;
}

///
///
///
GeosMultiPolygonPtr	GeosIO::newGeosMultiPolygon( MultiPolygon const& multi )
{
#ifdef GEOS_NO_UNIQUE_PTR
	std::vector< GeosGeometryPtr >* newGeoms = new std::vector< geos::geom::Geometry* >();
	for ( size_t i = 0; i < multi.numGeometries(); ++i ){
		newGeoms->push_back( newGeosPolygon( multi.polygonN(i) ) );
	}
#else
    std::vector< GeosGeometryPtr > newGeoms;
    for ( size_t i = 0; i < multi.numGeometries(); ++i ){
        newGeoms.push_back( newGeosPolygon( multi.polygonN(i) ) );
    }
#endif
    GeosMultiPolygonPtr mpg(_geometryFactory->createMultiPolygon( std::move(newGeoms) ));
	return mpg;
}
///
///
///
GeosMultiPolygonPtr	GeosIO::newGeosMultiPolygon( TriangulatedSurface const& triangulatedSurface )
{
#ifdef GEOS_NO_UNIQUE_PTR
	std::vector< GeosGeometryPtr >* newGeoms = new std::vector< geos::geom::Geometry* >();
	for ( size_t i = 0; i < triangulatedSurface.numTriangles(); ++i ){
		const Triangle &triangle=triangulatedSurface.triangleN(i);
		newGeoms->push_back( newGeosPolygon( triangle.toPolygon() ) );
	}
#else
    std::vector< GeosGeometryPtr > newGeoms;
    for ( size_t i = 0; i < triangulatedSurface.numTriangles(); ++i ){
        const Triangle &triangle=triangulatedSurface.triangleN(i);
        newGeoms.push_back( newGeosPolygon( triangle.toPolygon() ) );
    }
#endif
    GeosMultiPolygonPtr mpg(_geometryFactory->createMultiPolygon( std::move(newGeoms) ));
    return mpg;
}

///
///
///
GeosMultiPolygonPtr	GeosIO::newGeosMultiPolygon( PolyhedralSurface const& polyhedralSurface )
{
	return newGeosMultiPolygon( polyhedralSurface.toMultiPolygon() );
}

///
///
///
GeosGeometryCollectionPtr  GeosIO::newGeosGeometryCollection( GeometryCollection const& multi )
{
#ifdef GEOS_NO_UNIQUE_PTR
	std::vector< GeosGeometryPtr >* newGeoms = new std::vector< geos::geom::Geometry* >();
	for ( size_t i = 0; i < multi.numGeometries(); ++i ){
		newGeoms->push_back( newGeosGeometry( multi.geometryN(i) ) );
	}
#else
    std::vector< GeosGeometryPtr > newGeoms;
    for ( size_t i = 0; i < multi.numGeometries(); ++i ){
        newGeoms.push_back( newGeosGeometry( multi.geometryN(i) ) );
    }
#endif
    GeosGeometryCollectionPtr gc (_geometryFactory->createGeometryCollection( std::move(newGeoms) ));
	return gc;
}


///
///
///
void    GeosIO::fromGeosPoint( const geos::geom::Point* geometry, Point & out )
{
	out.clear();
	if ( !geometry || !geometry->getCoordinate() )
		return;
	out.setX( geometry->getCoordinate()->x );
	out.setY( geometry->getCoordinate()->y );
#ifdef GEOS_NO_UNIQUE_PTR
	if ( ! std::isnan( geometry->getCoordinate()->z ) )
#else
    if ( geometry->hasZ())
#endif
	{
		//out.setZ( geometry->getCoordinate()->z );
        out.setZ( geometry->getZ() );

	}
	//_bestPrecisionModel.snapToGrid( out );
}

///
///
///
Point	GeosIO::fromGeosCoordinate( GeosCoordinate const * geometry )
{
	Point pt;

	if ( geometry )
	{
		pt.setX( geometry->x );
		pt.setY( geometry->y );
		if ( ! std::isnan( geometry->z ) )
		{
			pt.setZ( geometry->z );
		}
		//_bestPrecisionModel.snapToGrid( pt );
	}

	return pt;
}

///
///
///
Point    GeosIO::fromGeosCoordinateXY( GeosCoordinateXY const * geometry )
{
    Point pt;

    if ( geometry )
    {
        pt.setX( geometry->x );
        pt.setY( geometry->y );
    }

    return pt;
}

///
///
///
Point*	GeosIO::newFromGeosPoint( const geos::geom::Point* geometry )
{
	Point * res = 0;
    
#ifdef GEOS_NO_UNIQUE_PTR
	if ( std::isnan( geometry->getCoordinate()->z ) ){
		res = new Point( geometry->getCoordinate()->x, geometry->getCoordinate()->y );
	}else{
		res = new Point( geometry->getCoordinate()->x, geometry->getCoordinate()->y, geometry->getCoordinate()->z );
	}
#else
    if ( !geometry->hasZ() ){
        res = new Point( geometry->getX(), geometry->getY() );
    }else{
        res = new Point( geometry->getX(), geometry->getY(), geometry->getZ() );
    }
#endif
	//_bestPrecisionModel.snapToGrid( *res );
	return res;
}


///
///
///
void	GeosIO::fromGeosLineString(const geos::geom::LineString* geometry, LineString& ls )
{
	ls.resize( geometry->getNumPoints() );
	for ( size_t i = 0; i < geometry->getNumPoints(); ++i ){
		//attention geometry->getPointN renvoie un clone
        ign::unique_ptr<geos::geom::Point> pCloneGeos = geometry->getPointN( i );
		fromGeosPoint( pCloneGeos.get(), ls.pointN(i) );
	}
}


///
///
///
LineString*	GeosIO::newFromGeosLineString( const geos::geom::LineString* geometry )
{
	LineString* ls = new LineString();
	fromGeosLineString( geometry, *ls );
	return ls;
}

///
///
///
void	GeosIO::fromGeosPolygon(const geos::geom::Polygon* geometry, ign::geometry::Polygon& polygon )
{
	if ( geometry->isEmpty() )
		return;

	//exteriorRing
	{
		fromGeosLineString( geometry->getExteriorRing(), polygon.exteriorRing() );
	}

	//interiorRings
	for ( size_t n = 0; n < geometry->getNumInteriorRing(); ++n ){
		polygon.addInteriorRing( LineString() );
		fromGeosLineString( geometry->getInteriorRingN(n), polygon.interiorRingN(n) );
	}
}

///
///
///
ign::geometry::Polygon* GeosIO::newFromGeosPolygon( const geos::geom::Polygon* geometry )
{
	ign::geometry::Polygon* polygon = new ign::geometry::Polygon();
	fromGeosPolygon( geometry, *polygon );
	return polygon;
}

///
///
///
void	 GeosIO::fromGeosMultiPoint(const geos::geom::MultiPoint * geometry, MultiPoint& multi )
{
	if ( geometry->isEmpty() ){
		return;
	}

	for ( size_t n = 0; n < geometry->getNumGeometries(); n++ ){
		Point p;
		fromGeosPoint(
			boost::polymorphic_cast< geos::geom::Point const *>( geometry->getGeometryN(n) ),
			p
		);
		multi.addGeometry(p);
	}
}

///
///
///
MultiPoint* GeosIO::newFromGeosMultiPoint( const geos::geom::MultiPoint* geometry )
{
	MultiPoint* multi = new MultiPoint();
	fromGeosMultiPoint( geometry, *multi );
	return multi;
}


///
///
///
void	 GeosIO::fromGeosMultiLineString(const geos::geom::MultiLineString* geometry, MultiLineString& multi )
{
	if ( geometry->isEmpty() ){
		return;
	}

	for ( size_t n = 0; n < geometry->getNumGeometries(); n++ ){
		LineString ls;
		fromGeosLineString(
			boost::polymorphic_cast< geos::geom::LineString const *>( geometry->getGeometryN(n) ),
			ls
		);
		multi.addGeometry(ls);
	}
}

///
///
///
MultiLineString* GeosIO::newFromGeosMultiLineString( const geos::geom::MultiLineString* geometry )
{
	MultiLineString* multi = new MultiLineString();
	fromGeosMultiLineString( geometry, *multi );
	return multi;
}

///
///
///
void	GeosIO::fromGeosMultiPolygon(const geos::geom::MultiPolygon* geometry, MultiPolygon& multi )
{
	if ( geometry->isEmpty() ){
		return;
	}

	for ( size_t n = 0; n < geometry->getNumGeometries(); n++ ){
		Polygon poly;
		fromGeosPolygon(
			boost::polymorphic_cast< geos::geom::Polygon const *>( geometry->getGeometryN(n) ),
			poly
		);
		multi.addGeometry(poly);
	}
}

///
///
///
MultiPolygon* GeosIO::newFromGeosMultiPolygon( const geos::geom::MultiPolygon* geometry )
{
	MultiPolygon* multi = new MultiPolygon();
	fromGeosMultiPolygon( geometry, *multi );
	return multi;
}

///
///
///
void	GeosIO::fromGeosGeometryCollection(const geos::geom::GeometryCollection* geometry, GeometryCollection& coll )
{
	if ( geometry->isEmpty() ){
		return;
	}

	for ( size_t n = 0; n < geometry->getNumGeometries(); n++ ){
		Geometry* item = newFromGeosGeometry( geometry->getGeometryN(n) );
		coll.addGeometryNoCopy( item );
	}
}

///
///
///
GeometryCollection* GeosIO::newFromGeosGeometryCollection( const geos::geom::GeometryCollection* geometry )
{
	GeometryCollection* collection = new GeometryCollection();
	fromGeosGeometryCollection( geometry, *collection );
	return collection;
}

///
///
///
geos::geom::GeometryFactory & 	GeosIO::geometryFactory()
{
	return *(_geometryFactory.get());
}

///
///
///
Geometry*	GeosIO::newFromGeosGeometry( const geos::geom::Geometry* geometry )
{
	if ( geometry == 0 )
		return new GeometryCollection();

	switch ( geometry->getGeometryTypeId() ){
	/// a point
		case geos::geom::GEOS_POINT:
		{
			return newFromGeosPoint( boost::polymorphic_cast< geos::geom::Point const * >(geometry) );
		}
		case geos::geom::GEOS_LINESTRING:
		{
			return newFromGeosLineString( boost::polymorphic_cast< geos::geom::LineString const * >(geometry) );
		}
		case geos::geom::GEOS_LINEARRING:
		{
			return newFromGeosLineString( boost::polymorphic_cast< geos::geom::LineString const * >(geometry) );
		}
		case geos::geom::GEOS_POLYGON:
		{
			return newFromGeosPolygon( boost::polymorphic_cast< geos::geom::Polygon const * >(geometry) );
		}
		case geos::geom::GEOS_MULTIPOINT:
		{
			return newFromGeosMultiPoint( boost::polymorphic_cast< geos::geom::MultiPoint const * >(geometry) );
		}
		case geos::geom::GEOS_MULTILINESTRING:
		{
			return newFromGeosMultiLineString( boost::polymorphic_cast< geos::geom::MultiLineString const * >(geometry) );
		}
		case geos::geom::GEOS_MULTIPOLYGON:
		{
			return newFromGeosMultiPolygon( boost::polymorphic_cast< geos::geom::MultiPolygon const * >(geometry) );
		}
		case geos::geom::GEOS_GEOMETRYCOLLECTION:
		{
			return newFromGeosGeometryCollection( boost::polymorphic_cast< geos::geom::GeometryCollection const * >(geometry) );
		}
	}
	IGN_THROW_EXCEPTION("[FATAL ERROR]Geometry*	GeosIO::newFromGeosGeometry( geos::geom::Geometry const * geometry )");
}

#endif //IGN_WITH_GEOS
