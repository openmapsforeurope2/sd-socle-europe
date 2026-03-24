#ifndef _SOCLE_GEOMETRY_GEOMETRY_H_
#define _SOCLE_GEOMETRY_GEOMETRY_H_

#include <limits>

#include <ign/ign_config.h>
#include <atomic>

#ifdef IGN_WITH_GEOS
    // necessaire car M_PI (defini dans math.h) est redefini dans le namespace geos par la lib Geos
    // donc si on ne le redefinit pas dans le namespace global, il devient indefini hors geos:: pour les autres modules

    #include <geos/constants.h>
    #ifndef M_PI
    #define M_PI geos::M_PI
    #endif
#endif

#include <string>
#include <vector>

#include <ign/common.h>
#include <ign/InstanceCounter.h>

#include <ign/data/Value.h>


namespace ign {

	namespace transform {
		class Transform;
	}

	namespace geometry {
		class Geometry;
		class Point;
		class LineString;
		class Polygon;
		class Triangle;
		class GeometryCollection;
		class MultiPoint;
		class MultiLineString;
		class MultiPolygon;
		class TriangulatedSurface;
		class PolyhedralSurface;

		class Envelope;
		class PrecisionModel;
		class GeometryMetadata;

		//class Transform;
		class GeometryVisitor;
		class ConstGeometryVisitor;

		/// \brief auto pointer on geometry
		typedef ign::unique_ptr< Geometry > GeometryPtr;
		/// \brief auto pointer on geometry
		typedef ign::shared_ptr< Geometry > GeometrySharedPtr;

	}
}


namespace ign {
	namespace geometry {
		typedef enum {
			RingOrientationClockWize 		=  1, //signe de l'aire algebrique correspondante
			RingOrientationAntiClockWize	= -1
		} RingOrientation ;
	}
}


/// \defgroup geometry_downcast_helper Aide au transtypage des geometries
/// \defgroup geometry_analysis Analyse des geometries (buffer, distance, etc...)
/// \defgroup geometry_query  Requete sur les geometries ( contient, intersecte, ... )



namespace ign {

	/// \brief Ce package regroupe les classes basees sur le standard OGC, representant les geometries.
	namespace geometry {

		/// \brief cette classe abstraite est la classe mere de toutes les geometries. Pour chacune
		///	des classes derivee, non abstraite, le constructeur par defaut construit une geometrie vide.
		///
		/// \todo attacher un modele de precision aux geometries de maniere independante
		class IGN_GEOMETRY_API Geometry : public data::Value {
		public:
			static std::atomic<int> liveCount;

			/// \brief la liste des types geometriques supportes.
			///	L'encodage du type correspond a l'encodage des SimpleFeature
			/// OGC/Simple Feature Access-Part 1 Common architecture ("A common list of codes for geometric types")
			/// OGC 06-103r4
			/// \warning CETTE LISTE EST AMENEE A EVOLUER
			typedef enum {
				GeometryTypeNull				= 0,
				GeometryTypePoint				= 1,
				GeometryTypeLineString			= 2,
				GeometryTypePolygon				= 3,
				GeometryTypeMultiPoint			= 4,
				GeometryTypeMultiLineString     = 5,
				GeometryTypeMultiPolygon		= 6,
				GeometryTypeGeometryCollection  = 7,
				GeometryTypeTriangle			= 8,
				GeometryTypeTriangulatedSurface	= 16,
				GeometryTypePolyhedralSurface	= 17
			} GeometryType ;

			typedef enum {
				CoordinateTypeUndefined			= 0,
				CoordinateTypeXY				= 1,
				CoordinateTypeXYZ				= 2,
				CoordinateTypeXYM				= 3,
				CoordinateTypeXYZM				= 4
			} CoordinateType ; 

			/// \brief represente la qualite du remplissage des coordonnees Z ou M
			typedef enum {
				/* Aucun point possede un Z */ 						CoordinateQualityNone,
				/* Il existe des points avec un Z, mais pas tous */ CoordinateQualityExists,
				/* Tous les points possede un Z */ 					CoordinateQualityAll
			} CoordinateQuality ;


			/// \brief renvoie le modele de precision attache a la geometrie. Si ce modele
			/// 		n'est pas definit, renvoie le modele de precision par defaut
			PrecisionModel const & getPrecisionModel() const;

			/// \brief Destructeur virtuel
			virtual ~Geometry();

			/// \brief Renvoie une copie conforme de la geometrie. Le constructeur par recopie est protege.
			virtual Geometry* 	 clone() const      = 0;

			/// \brief renvoie le type de la geometrie
			/// \warning anciennement getType
			virtual GeometryType getGeometryType() const = 0;
			/// \brief renvoie le type de coordonnes
			CoordinateType 		 getCoordinateType() const;
			/// \brief renvoie le nom des coordonnees (xy, xyz, xym, xyzm)
			std::string 		 getCoordinateTypeNames() const;

			/// \brief renvoie le type de la geometrie sous forme de texte
			/// \warning anciennement geometryType()
			std::string			 getGeometryTypeName() const;
			/// \brief renvoie la chaine de caracteres representant un type geometrique
			static std::string	 GeometryTypeName( GeometryType const& geometryType ) ;


			/// \brief indique si la geometrie est vide
			virtual bool		 isEmpty() const      = 0;
			/// \brief vide la geometrie
			virtual void		 clear( bool clearMetadata = true )	  = 0;		

			/// \brief renvoie la dimension des coordonnees
			int					 dimension () const ;
			/// \brief renvoie l'identifiant du systeme de reference spatial
			int const &			 SRID () const ;
			/// \brief definition du SRID
			void				 setSRID( int const& srid );

			/// \brief Renvoie vrai s'il EXISTE un point dans la geometrie possedant un Z. Toujours faux si isEmpty()
			///	\warning information calculee, pensez a la stocker pour optimiser.
			virtual bool				 is3D() const = 0;
			/// \brief Renvoie l'information sur le remplissage du Z dans la geometrie
			virtual CoordinateQuality    getCoordinateQualityZ() const = 0;
			/// \brief supprime les valeurs en Z (set to NaN)
			void						 clearZ() ;
			/// \brief definit un Z uniforme
			void						 setFillZ( double const& zValue ) ;

			/// \brief Renvoie vrai s'il EXISTE un point possedant un M dans la geometrie. Toujours faux si isEmpty()
			/// \warning information calculee, pensez a la stocker pour optimiser.
			virtual bool				 isMeasured() const = 0;
			/// \brief Renvoie l'information sur le remplissage du M dans la geometrie
			virtual CoordinateQuality    getCoordinateQualityM() const = 0;

			/// \brief calcul de la frontiere
			Geometry*					 boundary() const;

			/// \brief [2D/3D] calcul de l'envelope [2D/3D]
			virtual Envelope			 getEnvelope() const;
			/// \brief [2D/3D] calcul de l'envelope (a l'aide de expandToInclude dans bbox en parametre)
			virtual	void				 envelope( Envelope& bbox ) const = 0;

			/// \brief calcul le centroide
			/// \brief calcul Geos parfois bogue, utiliser CentroideOp pour calcul fiable en attendant
			Point					 	 getCentroid() const ;
			/// \brief renvoie le centroide de la surface
			/// \brief deprecated, use get centroid
			Point   					 centroid() const ;

			/// \brief calcul d'un point a l'interieur de la geometrie
			Point					 	 getInteriorPoint() const ;

			/// \brief renvoie la chaine de caracteres WKT
			std::string				     asText( int const& numDecimals = -1 ) const;

			///
			/// \brief Convertit en multi-geometrie (point => multipoint, linestring => multilinestring, etc.)
			/// Renvoie une copie profonde sinon.
			virtual Geometry* 			toMulti() const ;

			
			/// \brief permet de remettre les metadonnees aux valeurs par defaut (srid=-1)
			void						clearMetadata();
			
			
			/// \brief apply a visitor on geometry
			virtual void				accept( GeometryVisitor & visitor ) = 0 ;
			/// \brief apply a visitor on a const geometry
			virtual void				accept( ConstGeometryVisitor & visitor ) const = 0 ;

			// -- Aide pour decouvrir le type

			/// \brief indique s'il s'agit d'un point
			/// \ingroup geometry_downcast_helper
			bool				 isPoint() const;
			/// \brief indique s'il s'agit d'une polyligne
			/// \ingroup geometry_downcast_helper
			bool				 isLineString() const;
			/// \brief indique s'il s'agit d'un polygon
			/// \ingroup geometry_downcast_helper
			bool				 isPolygon() const;
			/// \brief indique s'il s'agit d'un triangle
			/// \ingroup geometry_downcast_helper
			bool				 isTriangle() const;
			/// \brief indique s'il s'agit d'une triangulatedSurface
			/// \ingroup geometry_downcast_helper
			bool				 isTriangulatedSurface() const;
			/// \brief indique s'il s'agit d'une PolyhedralSurface
			bool				 isPolyhedralSurface() const ;

			/// \brief indique s'il s'agit d'un multi-point
			/// \ingroup geometry_downcast_helper
			bool				 isMultiPoint() const;
			/// \brief indique s'il s'agit d'une multi-polyligne
			/// \ingroup geometry_downcast_helper
			bool				 isMultiLineString() const;
			/// \brief indique s'il s'agit d'un multi-polygone
			/// \ingroup geometry_downcast_helper
			bool				 isMultiPolygon() const;
			/// \brief indique s'il s'agit d'une collection de geometrie
			///	 ou d'une classe derivee.
			/// \ingroup geometry_downcast_helper
			bool				 isGeometryCollection() const;


			/// \brief downcast en point
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual Point const&			   asPoint() const;
			/// \brief downcast en point
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual Point&					   asPoint();
			/// \brief downcast en polyligne
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual LineString const&		   asLineString() const;
			/// \brief downcast en polyligne
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual LineString&				   asLineString();
			/// \brief downcast en polygone
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual Polygon const&		       asPolygon() const;
			/// \brief downcast en polygone
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual Polygon&				   asPolygon();
			/// \brief downcast en triangle
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual Triangle const&		       asTriangle() const;
			/// \brief downcast en triangle
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual Triangle&				   asTriangle();
			/// \brief downcast en multipoint
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual MultiPoint const&		   asMultiPoint() const;
			/// \brief downcast en multipoint
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual MultiPoint&				   asMultiPoint();
			/// \brief downcast en multilinestring
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual MultiLineString const&	   asMultiLineString() const;
			/// \brief downcast en multilinestring
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual MultiLineString&		   asMultiLineString();
			/// \brief downcast en multipolygone
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual MultiPolygon const&		   asMultiPolygon() const;
			/// \brief downcast en multipolygone
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual MultiPolygon&			   asMultiPolygon();
			/// \brief downcast en collection de geometrie
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual GeometryCollection const&  asGeometryCollection() const;
			/// \brief downcast en collection de geometrie
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual GeometryCollection&		   asGeometryCollection();
			/// \brief downcast en surface triangulee
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual TriangulatedSurface const&	asTriangulatedSurface() const ;
			/// \brief downcast en surface triangulee
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual TriangulatedSurface&		asTriangulatedSurface();
			/// \brief downcast en PolyhedralSurface
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual PolyhedralSurface const &	asPolyhedralSurface() const ;
			/// \brief downcast en PolyhedralSurface
			/// \throw BadGeometryCast
			/// \ingroup geometry_downcast_helper
			virtual PolyhedralSurface&			asPolyhedralSurface();

			/// \brief fonction utilitaire permettant de fusionner les qualites de remplissage des coordonnees
			/// \warning les geometries s'en charge, utile seulement sur un ensemble de geometries
			static CoordinateQuality   MixQualities( std::vector< CoordinateQuality > const& qualities );

			/// \brief definit recursivement les metadonnees associee a la geometrie (ne prend pas la responsabilite
			/// du pointeur GeometryMetadata)
			void	setGeometryMetadata( GeometryMetadata * geometryMetadata, bool recursive = true ) ;

			/// \brief get geometry metadata (GeometryMetadata::DefaultMetadata if not set)
			GeometryMetadata const & getGeometryMetadata() const ;

			
			/// \brief indique si la geometrie a un sens (contours de polygone fermes, nombres de points suffisant ou vide, etc...)
			/// \todo ajouter tests unitaires
			virtual bool			   isValid() const ;
			
			/// \brief test de la validite d'une geometrie (GEOS)
			/// \todo virer l'algorithme dans ign::geometry::algorithm::IsSimpleOpGeos
			bool						isSimple() const;

			//-- [ign::data::Value]
			virtual std::string toString() const ;

			//-- [ign::data::Value]
			virtual data::Value::ValueType const&  getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const& 	           getValueTypeName() const ;
			
            /// \brief applique une transformation sur chacun des points
			void						applyTransform( transform::Transform const & transform , bool const direct = true);

#ifdef IGN_WITH_GEOS
			
			
			/// \brief Test d'egalite avec une autre geometrie (GEOS)
			/// \ingroup geometry_query
            /// to avoid clan warning about 'ign::geometry::Geometry::equals' hides overloaded virtual function' make an explicit overload
            using Value::equals;
			bool						 equals( Geometry const& other ) const;
			/// \brief Test si les geometries sont disjointes (GEOS)
			/// The opposite of ST_Intersects is disjoint(geometry A , geometry B).
			/// If two geometries are disjoint, they do not intersect, and vice-versa. 
			/// In fact, it is often more efficient to test 'not intersects' than to test 
			/// 'disjoint' because the intersects tests can be spatially indexed, while 
			/// the disjoint test cannot.
			/// \ingroup geometry_query
			bool						 disjoint( Geometry const& other ) const;
			/// \brief Test de l'intersection de la geometrie (GEOS)
			/// intersects(geometry A, geometry B) returns t (TRUE) if the intersection 
			/// does not result in an empty set. Intersects returns the exact opposite 
			/// result of disjoint.
			/// \ingroup geometry_query
			bool						 intersects( Geometry const& other ) const;
			/// \brief Test si la geometrie touche celle en parametre (GEOS)
			/// touches(geometry A, geometry B) returns TRUE if either of the geometrie's 
			/// boundaries intersect or if only one of the geometry's interiors intersects 
			/// the other's boundary.
			/// \ingroup geometry_query
			bool						 touches( Geometry const& other ) const;
			/// \brief Test si la geometrie croise celle en parametre (GEOS)
			/// For multipoint/polygon, multipoint/linestring, linestring/linestring, 
			/// linestring/polygon, and linestring/multipolygon comparisons, 
			/// crosses(geometry A, geometry B) returns t (TRUE) if the intersection 
			/// results in a geometry whose dimension is one less than the maximum dimension 
			/// of the two source geometries and the intersection set is interior to both 
			/// source geometries.
			/// \ingroup geometry_query
			bool						 crosses( Geometry const& other ) const;
			/// \brief Test si la geometrie est a l'interieur de celle en parametre (GEOS)
			/// within(geometry A , geometry B) returns TRUE if the first geometry is 
			/// completely within the second geometry. ST_Within tests for the exact opposite 
			/// result of ST_Contains.
			/// \ingroup geometry_query
			bool						 within( Geometry const& other ) const;
			/// \brief Test si la geometrie contient celle en parametre (GEOS)
			/// contains(geometry A, geometry B) returns TRUE if the second geometry is 
			/// completely contained by the first geometry.
			/// \ingroup geometry_query
			bool						 contains( Geometry const& other ) const;
			/// \brief Test si la geometrie se superpose a celle en parametre (GEOS)
			/// overlaps(geometry A, geometry B) compares two geometries of the same 
			/// dimension and returns TRUE if their intersection set results in a geometry 
			/// different from both but of the same dimension.
			/// \ingroup geometry_query
			bool						 overlaps( Geometry const& other ) const;
			
			/// \brief Returns true if the elements in the DE-9IM intersection 
			/// matrix for the two Geometrys match the elements in intersectionPattern.
			/*
			 * Matrix DE-9IM :
			 *                  Interior   |    Boundary    |     Exterior
			 *       Interior
			 *       Boundary
			 *       Exterior
			 */
			/// The pattern matrix consists of a set of nine pattern-values, one for each 
			/// cell in the matrix. The possible pattern-values of p are {T, F, *, 0, 1, 2} 
			/// The pattern matrix consists of a set of nine pattern-values, one for each 
			/// cell in the matrix. The possible pattern-values of p are {T, F, *, 0, 1, 2} 
			/// and their meanings for any cell where x is the intersection set for the cell 
			/// are as follows:
			///    p = T -> dim(x) = {0, 1, 2}, i.e. x != null
			///    p = F -> dim(x) = -1, i.e. x = null
			///    p = * -> dim(x) = {-1, 0, 1, 2}, i.e. Don't Care
			///    p = 0 -> dim(x) = 0
			///    p = 1 -> dim(x) = 1
			///    p = 2 -> dim(x) = 2
			bool						 relate( Geometry const& other, const std::string& intersectionPattern ) const;


			/// \brief calcul de la distance 2D a une autre geometrie (GEOS)
			/// \warning ne prend pas en compte la 3D
			/// \return distance dans l'unite des coordonnees, valeur negative si le calcul est impossible
			/// \ingroup geometry_analysis
			double					   distance( Geometry const& other ) const;

			/// \brief calcul du buffer autour d'une geometrie (GEOS)
			/// \ingroup geometry_analysis
			Geometry*				   buffer( double distance, size_t n = 8 ) const;
			
			/// \brief renvoie l'enveloppe convexe de la geometrie (GEOS)
			/// \ingroup geometry_analysis
			Geometry*				   convexHull() const;
			
			/// \brief calcul de l'union
			/// \ingroup geometry_analysis
			Geometry*				   Union( Geometry const& other ) const;
			/// \brief calcul de l'intersection
			/// \ingroup geometry_analysis
			Geometry*				   Intersection( Geometry const& other ) const ;
			/// \brief calcul de la difference
			/// \ingroup geometry_analysis
			Geometry*				   Difference( Geometry const& other ) const ;
			/// \brief calcul de la difference symetrique
			/// \ingroup geometry_analysis
			Geometry*				   SymDifference( Geometry const& other ) const ;
			
			/// Converts this Geometry to normal form (or canonical form).
			Geometry*					normalize() const;
			
			/// \brief indique si l'objet est a moins de "distance" de "other" (GEOS)
			/// \todo voir ce qui plante, les tests echouent
			/// \warning non-ogc
			bool					   isWithinDistance( Geometry const& other, double const& distance ) const;
#endif			
			

		protected:
			/// \brief reference vers les metadonnees d'un groupe de geometrie (non owner)
			GeometryMetadata	*	_geometryMetadata;

			/// \brief constructeur par defaut protege
			Geometry();
			
			/// \brief recuperation des metadonnees a partir d'une autre geometrie
			void copyMetadataFrom( Geometry const& other ); 

			//-- [ign::data::Value]
			virtual void _asJson( std::ostream & s ) const ;
		private:
			/// \brief constructeur par recopie prive (copie interdite)
			/// \see clone()
			Geometry( Geometry const& other );

			/// \brief envoie d'une exception pour une erreur de cast
			std::string		badCastExceptionMessage( std::string const& from, std::string const& to ) const;
		};
	}
}


#endif
