#cmake_minimum_required( VERSION 2.8.3 )
cmake_minimum_required(VERSION 3.20)
project(IgnSocle)
STRING(TOUPPER IgnSocle PROJECT_NAME_UPPER)
STRING(TOLOWER IgnSocle PROJECT_NAME_LOWER)

# Politiques encore pertinentes
cmake_policy(SET CMP0017 NEW)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0057 NEW)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
    cmake_policy(SET CMP0074 NEW)
endif()

ENABLE_TESTING()
#-----------------------------------------------------------------------------------------------------
# Informations
#    boost
#        - Seule les versions de BOOST superieures a 1.46.1 sont supportees (filesystem v3)
#        - La variable d'environnement BOOST_ROOT doit etre definie sur extern/win32-g++/boost_1_xx_x (ou macosx, unix-g++, etc.)
#        - Les composants suivants sont utilises : thread, test ( test_exec_monitor, unit_test_framework ), system, filesystem, serialization, program_option
#
# Options de compilation
#
#    Vous pouvez definir CMAKE_BUILD_TYPE a "Debug" ou "Release"
#
#    Toutes les dependences sont recherchees a l'aide de find_package. Vous pouvez definir
#        SOCLE_EXTERN_DIR pour specifier le dossier contenant le fichier "SocleExterns.cmake"
#        qui contient la definition de l'emplacement des externes
#
#    MODULES
#        La compilation de chaque module est pilotee par les variables CMake IgnSocle_WITHOUT_${MODULE_NAME}, par defaut a FALSE (tout est compile par defaut)
#
#    0-  Tronc commun 'core'
#        Quelques APIs compilees par defaut  qui ne dependent que de la STL (evt de Boost)
#
#    1-  Module numeric
#        say something about it...
#
#    2-  Module tools
#        say something about it...
#
#    3-  Module xml
#        say something about it...
#
#    4-  Module math
#        say something about it...
#
#    5-  Module data
#        say something about it...
#
#    6-  Module transform
#        -> dependances: math
#
#    7-  Module geodesy
#        say something about it...
#
#    8- Module filesystem
#        say something about it...
#
#    9- Module geometry
#        say something about it...
#
#    10- Module sql
#        say something about it...
#
#    11- Module feature
#        say something about it...
#
#    12- Module graph
#        say something about it...
#
#    13- Module shapefile
#        say something about it...
#
#    14- Module orientation
#        -> dependances: math, xml, matis
#        Le module Orientation est compile si la variable IgnSocle_WITHOUT_ORIENTATION n'est pas defini
#
#    15- Module image
#        -> dependances: math, xml, tools, data, transform, geodesy
#        Le module IMAGE est compilee si la variable IgnSocle_WITHOUT_IMAGE n'est pas defini
#        Si la variable IgnSocle_WITH_GDAL est a 'ON' et que GDAL est trouve le module est compile avec le driver GDAL
#        Si KAKADU est trouve la variable IgnSocle_WITH_KAKADU est definie et le module est compile avec le driver KAKADU
#        Si QT est trouve la variable IgnSocle_WITH_QT est definie et le module est compile avec le widget QT
#        Si le module ORIENTATION est compile, la variable IgnSocle_WITH_ORIENTATION est definie et le module est compile avec les classes d'images orientees
#
#    16- Module lib3d
#        Le module Lib3d est compile si la variable IgnSocle_WITHOUT_LIB3D n'est pas defini OU si le package IgnTvv n'est pas trouve
#
#    17- Module altimetry
#        Le module Altimetry est compile si la variable IgnSocle_WITHOUT_ALTIMETRY n'est pas defini
#        si le module GEOMETRY est compile, les classes MNT TIN sont compilees, sinon, seuls les classes permettant de gerer un MNT raster sont compiles
#
#    18- Module photogrammetry
#        Le module photogrammetry est compile si la variable IgnSocle_WITHOUT_PHOTOGRAMMETRY n'est pas defini
#
#    19- Module radiometry
#        Le module radiometry est compile si la variable IgnSocle_WITHOUT_RADIOMETRY n'est pas defini
#
#    20- Module aero
#        Le module aero est compile si la variable IgnSocle_WITHOUT_AERO n'est pas defini
#
#    21- Module gpao - deux lib gpao et gpao_datamodel
#        -> dependances QT : si non, seul gpao_datamodel est compilée
#        Le module gpao est compile si la variable IgnSocle_WITHOUT_GPAO n'est pas defini
#
#    22- Module log
#        Le module log est compile si la variable IgnSocle_WITHOUT_LOG n'est pas definie.
#
#
#    EXTENSIONS
#        Les extensions sont des modules optionnels dependants de librairies non standard et/ou non libres de droit
#        Elles sont placees dans le dossier extension afin de pouvoir les supprimer facilement lors d'une livraison
#        La syntaxe des options pour activer les extensions est IgnSocle_BUILD_(EXTENSION_NAME_UPPER)_EXTENSION _PROJECT_EXTENSION}_UPPER}" OFF )
#
#        Extension umfpack
#        L'extension umfpack est compilee si IgnSocle_BUILD_UMFPACK_EXTENSION est a ON
#
#        Extension CGAL
#        L'extension CGAL est compilee si IgnSocle_BUILD_CGAL_EXTENSION est a ON
#
#        Extension gts
#        L'extension GTS est compilee si IgnSocle_BUILD_GTS_EXTENSION est a ON
#
#        Extension shewchuk
#        L'extension shewchuk est compilee si IgnSocle_BUILD_SHEWCHUK_EXTENSION est a ON
#
#        Extension xtool2
#        L'extension xtool2 est compilee si IgnSocle_BUILD_XTOOL2_EXTENSION est a ON
#
#        Extension reco3D
#        L'extension reco3D est compilee si IgnSocle_BUILD_RECO3D_EXTENSION est a ON
#
#        Extension osg
#        L'extension osg est compilee si IgnSocle_BUILD_OSG_EXTENSION est a ON
#
#        Extension ossature
#        L'extension ossature est compilee si IgnSocle_BUILD_OSSATURE_EXTENSION est a ON
#
#        Extension classification
#        L'extension ossature est compilee si IgnSocle_BUILD_CLASSIFICATION_EXTENSION est a ON
#
#    OPTIONS SUPPLEMENTAIRES
#        Autres options sur les modules
#
#        Les tests unitaires sont compiles si IgnSocle_BUILD_MODULE_TESTS est a ON (par defaut) make test permet de les lancer
#
#        Les tests de non regression sont compiles si IgnSocle_BUILD_MODULE_TNR est a ON (OFF par defaut) permettant de verifier la non regression du code
#
#        Les tools sont compiles si IgnSocle_BUILD_MODULE_TOOLS est a ON (par defaut)
#
#        On genere des headers precompiles si la variable IgnSocle_USE_PRECOMPILED_HEADERS est a ON (OFF par defaut)
#--------------------------------------------------------------------------------------------------


# TODO : supprimer cette variable (le support C++11 n'est plus optionnel)
set(IGN_WITH_CXX11 ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-frounding-math -fno-fast-math)
IF (WIN32)
    add_compile_options(/Zc:__cplusplus)
ENDIF()

# Ajout des finders cmake personnalisés
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# -------------------------------------------------------------------
# Détection et verrouillage de l’environnement Conda / Micromamba
# -------------------------------------------------------------------
if(DEFINED ENV{CONDA_PREFIX} AND EXISTS "$ENV{CONDA_PREFIX}")
    set(CONDA_PREFIX "$ENV{CONDA_PREFIX}" CACHE PATH "Active Conda environment prefix" FORCE)

    message(STATUS "Using Conda environment: ${CONDA_PREFIX}")

    # Forcer CMake à résoudre dépendances, headers et libs depuis l'env actif
    set(CMAKE_PREFIX_PATH "${CONDA_PREFIX}" CACHE PATH "" FORCE)
    set(CMAKE_INCLUDE_PATH "${CONDA_PREFIX}/include" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_PATH "${CONDA_PREFIX}/lib" CACHE PATH "" FORCE)

    # S'assurer que les includes de l'env passent AVANT tout le reste
    include_directories(BEFORE SYSTEM "${CONDA_PREFIX}/include")
    link_directories("${CONDA_PREFIX}/lib")

else()
    message(FATAL_ERROR
        "CONDA_PREFIX not set or invalid. "
        "Please activate the Conda/Micromamba environment before running CMake."
    )
endif()

#------------------------------------------------------------------------
# Configuration du projet (definit la liste des modules et extensions)
#------------------------------------------------------------------------

#-- definition de la version de la bibliotheque
include( IgnSocleConfig.cmake )

#------------------------------------------------------------------------
# hack cmake pour la definition du repertoire d'installation
#------------------------------------------------------------------------
#IF(NOT IgnSocle_CONFIG_HAS_BEEN_RUN_BEFORE)
#    UNSET(CMAKE_INSTALL_PREFIX CACHE)
#    SET(CMAKE_INSTALL_PREFIX ${IgnSocle_SOURCE_DIR} CACHE PATH "")
#ENDIF()

# ign-<major>.<minor>.<patch>d if Debug, else ign-<major>.<minor>.<patch>
SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")

#------------------------------------------------------------------------------------
# Configuration du compilateur
#------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Definition des modules a compiler
#------------------------------------------------------------------------
#-- definition des options permettant de controler les modules / extensions a compiler


set(IGN_WITH_PROJ 1)
set(IGN_PROJ_VERSION 9)


foreach( IgnSocle_PROJECT_MODULE ${IgnSocle_PROJECT_MODULES} )
    STRING(TOUPPER ${IgnSocle_PROJECT_MODULE} ${IgnSocle_PROJECT_MODULE}_UPPER)
    OPTION( IgnSocle_WITHOUT_${${IgnSocle_PROJECT_MODULE}_UPPER} "activer/desactiver la compilation du module ${${IgnSocle_PROJECT_MODULE}_UPPER}" OFF )
endforeach()


foreach( IgnSocle_PROJECT_EXTENSION ${IgnSocle_PROJECT_EXTENSIONS} )
    STRING(TOUPPER ${IgnSocle_PROJECT_EXTENSION} ${IgnSocle_PROJECT_EXTENSION}_UPPER)
    OPTION( IgnSocle_BUILD_${${IgnSocle_PROJECT_EXTENSION}_UPPER}_EXTENSION "activer/desactiver la compilation du module ${${IgnSocle_PROJECT_EXTENSION}_UPPER}" OFF )
endforeach()


#-- activation/desaction des tests unitaires
OPTION( IgnSocle_BUILD_MODULE_TESTS "build a project to run unit tests" ON )
#-- activation/desaction des tests de non regression
OPTION( IgnSocle_BUILD_MODULE_TNR "build a project to run non regression tests" OFF )
#-- activation/desaction des tools
OPTION( IgnSocle_BUILD_MODULE_TOOLS "build a project to compile simple tools based on each library" ON )
#-- activation/desaction des examples
OPTION( IgnSocle_BUILD_EXAMPLES "build the examples" OFF )
#-- activation/desaction des headers precompiles
OPTION( IgnSocle_USE_PRECOMPILED_HEADERS "build the precompiled headers" OFF )
#-- activation/desactivation du wrapping python
OPTION( IgnSocle_BUILD_BOOST_PYTHON_MODULE "build the boost python wrapping" OFF )

OPTION (IgnSocle_TEST_COVERAGE "Generer la couverture des tests" OFF)

#-- activatioset(CMAKE_SKIP_BUILD_RPATH TRUE)n/desactivation du wrapper python
#OPTION( IgnSocle_WRAPPER_PYTHON "build wrapper python files" OFF )
#-- activation/desactivation du wrapper java
#OPTION( IgnSocle_WRAPPER_JAVA "build wrapper java files" OFF )

#IF( IgnSocle_WRAPPER_PYTHON OR IgnSocle_WRAPPER_JAVA )
#    SET(SWIG_EXECUTABLE "/usr/local/bin/swig")
#    FIND_PACKAGE(SWIG 3.0.5 REQUIRED)
#    INCLUDE(${SWIG_USE_FILE})
#ENDIF()

IF (IgnSocle_TEST_COVERAGE)
    IF (NOT CMAKE_COMPILER_IS_GNUCXX)
        message("-- warning: Couverture de test desactivee car seulement sous unix --")
        set(IgnSocle_TEST_COVERAGE OFF)
    ENDIF()
    IF (NOT IgnSocle_BUILD_MODULE_TESTS)
        message("-- warning: Couverture de test desactivee car la compilation des tests est desactivee --")
        set(IgnSocle_TEST_COVERAGE OFF)
    ENDIF()
ENDIF()

if(IgnSocle_TEST_COVERAGE)
    
    include("CodeCoverage")
    
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
    
    add_definitions(-DCOUVERTURE_TEST=1)
    
endif()

IF( IgnSocle_BUILD_BOOST_PYTHON_MODULE )
        MESSAGE("Activation du wrapper python")
        FIND_PACKAGE(PythonLibs 2.7.4 EXACT)
        INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
ENDIF()


#precompiling if option activated
IF ( NOT ${IgnSocle_USE_STATIC_LIBS} )
    # Add "-fPIC" for shared library build
    # TODO : available for msvc
    set( pch_option "-fPIC" )
ELSE()
        set( pch_option "" )
ENDIF()

#------------------------------------------------------------------------
# Recherche des dependences et definition des repertoire d'include
#   pour les bibliotheques trouvees
#------------------------------------------------------------------------

include_directories( include )

#on privilegie les librairies static ou dynamique selon l'option BUILD_SHARED_LIBS
IF ( BUILD_SHARED_LIBS)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .dylib  .so .lib .a)
ELSE()
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib .dylib .so)
ENDIF()

#does not work, cf https://stackoverflow.com/questions/3801011/ld-library-not-found-for-lcrt0-o-on-osx-10-6-with-gcc-clang-static-flag
#IF ( NOT BUILD_SHARED_LIBS)
#    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
#ENDIF()

# -------------------------------------------------------------------
# Boost
# -------------------------------------------------------------------
option( Boost_USE_AUTO_LINK "boost use autolink" OFF )
if( NOT ${Boost_USE_AUTO_LINK} )
    add_definitions( "-DBOOST_ALL_NO_LIB" )
endif()

option( Boost_USE_MULTITHREAD "boost use multithread libraries" ON )
if( ${Boost_USE_MULTITHREAD} )
    message( STATUS "Boost_USE_MULTITHREAD=ON" )
else()
    message( STATUS "Boost_USE_MULTITHREAD=OFF" )
endif()

option( Boost_USE_STATIC_LIBS "boost use static libraries" OFF )
option( Boost_USE_STATIC_RUNTIME "boost use static libraries" OFF )

if(Boost_USE_STATIC_LIBS)
    message(STATUS "building IgnSocle with static Boost libraries...")
    add_compile_definitions(BOOST_THREAD_USE_LIB)
else()
    message(STATUS "building IgnSocle with dynamic Boost libraries...")
    add_compile_definitions(BOOST_TEST_DYN_LINK)
    add_compile_definitions(BOOST_ALL_DYN_LINK)
endif()


set (Boost_DETAILED_FAILURE_MSG TRUE)

add_compile_definitions(BOOST_FILESYSTEM_VERSION=4)


set(BOOST_ROOT ${CONDA_PREFIX})
set(BOOST_INCLUDEDIR ${CONDA_PREFIX}/include)
set(BOOST_LIBRARYDIR ${CONDA_PREFIX}/lib)

# Indique les composants nécessaires selon ton projet
if( IgnSocle_BUILD_BOOST_PYTHON_MODULE )
    find_package( Boost 1.82.0
                COMPONENTS date_time filesystem system serialization thread  program_options regex timer chrono python unit_test_framework
                REQUIRED
    )
else()
    find_package( Boost 1.82.0
                COMPONENTS date_time filesystem system serialization thread  program_options regex timer chrono unit_test_framework
                REQUIRED
    )
endif()


if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS} ")
    message(STATUS " - Boost library dir: ${Boost_LIBRARY_DIRS}")
    message(STATUS " - Boost libraries: ${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "Boost not found in ${CONDA_PREFIX}")
endif()


# system avoids warnings in boost code
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

find_package(TinyXML REQUIRED)
if (TinyXML_FOUND)
    message( STATUS "building IgnSocle with TinyXML ${TinyXML_VERSION}..." )
    # activation utilisation de la STL dans TinyXML
    add_definitions( "-DTIXML_USE_STL" )
    # variables pour ign_config.h
    set( IGN_WITH_TINYXML "ON" )
else()
#    message( STATUS "building IgnSocle without TinyXML..." )
 find_package(tinyxml2 REQUIRED)
 target_link_libraries(${PROJECT_NAME} PUBLIC tinyxml2::tinyxml2)
endif()


#-- recherche de shapelib (optionnelle)
find_package( Shapelib )
if( Shapelib_FOUND )
    # variables pour ign_config.h
    set( IGN_WITH_SHAPELIB "ON" )
else()
    message(STATUS "Shapelib not found [ Shapelib_DIR: ${Shapelib_DIR}; SHAPELIB_ROOT: $ENV{SHAPELIB_ROOT} ]")
endif()

find_package(TIFF REQUIRED)

find_package(PROJ REQUIRED)

# -- Qt ---------------------------------------------------------------
option(IgnSocle_WITH_QT "Activer/désactiver la dépendance à Qt" ON)

if(IgnSocle_WITH_QT)
    # Backup CMAKE_PREFIX_PATH
    set(CMAKE_PREFIX_PATH_BCKP ${CMAKE_PREFIX_PATH})
    if(Qt5_DIR)
        set(CMAKE_PREFIX_PATH ${Qt5_DIR}/lib/cmake)
    endif()

    # Find Qt5 modules (PrintSupport, DBus inclus si nécessaire)
    find_package(Qt5 COMPONENTS Core Gui Widgets Sql Network PrintSupport DBus REQUIRED)

    # Restore CMAKE_PREFIX_PATH
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH_BCKP})

    if(Qt5_FOUND)
        set(IGN_WITH_QT ON)
        set(IGN_QT_VERSION 5)
        set(QT_FOUND ON)
        message(STATUS "Building IgnSocle with Qt${IGN_QT_VERSION} ${Qt5_VERSION}...")

        # Include directories
        set(QT_INCLUDE_DIRS
            ${Qt5Core_INCLUDE_DIRS}
            ${Qt5Gui_INCLUDE_DIRS}
            ${Qt5Widgets_INCLUDE_DIRS}
            ${Qt5Sql_INCLUDE_DIRS}
            ${Qt5Network_INCLUDE_DIRS}
            ${Qt5PrintSupport_INCLUDE_DIRS}
            ${Qt5DBus_INCLUDE_DIRS}
        )

        # Qt libraries
        set(QT_LIBRARIES
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
            Qt5::Sql
            Qt5::Network
            Qt5::PrintSupport
            Qt5::DBus
        )

        # Qt library directories (utile pour Windows)
        foreach(_lib IN LISTS QT_LIBRARIES)
            get_target_property(_loc ${_lib} LOCATION)
            if(_loc)
                get_filename_component(_dir ${_loc} PATH)
                list(APPEND QT_LIBRARY_DIRS ${_dir})
            endif()
        endforeach()

        # Automatic moc, uic, rcc
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTOUIC ON)
        set(CMAKE_AUTORCC ON)

    else()
        set(IGN_WITH_QT OFF)
        message(WARNING "Unable to detect Qt5, disabling all Qt5-dependent modules")
    endif()
else()
    message(STATUS "IgnSocle compiled without Qt5-dependent modules")
endif()



#-- recherche de GDAL (image et dependances)
#set (CMAKE_FIND_DEBUG_MODE ON)
find_package( GDAL 3 REQUIRED)
set (IGN_WITH_GDAL "ON")
#set (CMAKE_FIND_DEBUG_MODE OFF)

#-- recherche de CGAL (extension cgal + reco3d )
OPTION(IgnSocle_WITH_CGAL "activer/desactiver la dependance a CGAL" OFF )
IF(IgnSocle_BUILD_CGAL_EXTENSION)

    # --- Snapshot de Boost_LIBRARIES pour préserver la valeur avant que CGAL ne la réinitialise ---
    set(_BOOST_LIBRARIES_SAVE ${Boost_LIBRARIES})

    set (CGAL_ADDITIONAL_VERSIONS "6.0")
    # --- Recherche CGAL ---
    find_package(
        CGAL
        COMPONENTS Core
        REQUIRED
    )
    add_definitions(-DCGAL_USE_BASIC_VIEWER -DCGAL_HEADER_ONLY)
    
    # --- Restore de Boost_LIBRARIES pour conserver les valeurs historiques après CGAL ---
    set(Boost_LIBRARIES ${_BOOST_LIBRARIES_SAVE})

    find_package( GMP )
    find_package( MPFR )
    if ( CGAL_FOUND AND MPFR_FOUND AND GMP_FOUND)
        set(IgnSocle_WITH_CGAL ON)
        #Don't let CGAL override flags
        set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE CACHE BOOL "Force CGAL to maintain CMAKE flags")
        message ("CGAL_VERSION: ${CGAL_VERSION}")
        message ("CGAL_VERSION_MAJOR: ${CGAL_VERSION_MAJOR}")
        # Inclut CGAL_USE_FILE seulement si CGAL major <= 5
        if(CGAL_VERSION_MAJOR LESS_EQUAL 5)
            include(${CGAL_USE_FILE})
            message(STATUS "Included CGAL_USE_FILE for CGAL <= 5")
        else()
            message(STATUS "Skipping CGAL_USE_FILE for CGAL >= 6")
        endif()
        
        add_definitions( "-DCGAL_USE_BOOST_FILESYSTEM" )
        list(APPEND CGAL_3RD_PARTY_LIBRARIES ${Boost_FILESYSTEM_LIBRARY} )
        set (CGAL_DEPENDENCIES     CGAL  MPFR  GMP  )
    else()
        set(IgnSocle_WITH_CGAL OFF)
        set(IgnSocle_BUILD_CGAL_EXTENSION OFF)
        message("-- warning: extension CGAL non compilee car l'une de ces librairies n'a pas ete trouvee: CGAL (${CGAL_FOUND}), MPFR (${MPFR_FOUND}), GMP (${GMP_FOUND}) --")
    endif()
ENDIF()
IF (IgnSocle_WITH_CGAL)
        # variables pour ign_config.h
        set( IGN_WITH_CGAL "ON" )
  
        #fix link cgal
        if("${CGAL_MAJOR_VERSION}" STREQUAL "5")
            SET(CGAL_LIBRARIES "")
            SET(CGAL_LIBRARY_DIRS "")
        endif()
ENDIF()


#-- recherche de GEOS (geometry et dependances)
set( Geos_USE_C_API "OFF" )
IF(APPLE)
    #besoin de la lib C pour Geos sous MacOSX
    IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        SET(Geos_USE_C_API "ON")
    ENDIF()
ENDIF()

find_package( Geos REQUIRED)
message ("Geos_FOUND: ${Geos_FOUND}")

if( Geos_FOUND )
    # variables pour ign_config.h
    set( IGN_WITH_GEOS "ON" )
    # add_definitions(-DGEOS_VERSION_MAJOR=${Geos_VERSION_MAJOR})
    # add_definitions(-DGEOS_VERSION_MINOR=${Geos_VERSION_MINOR})
 
else()
    message(FATAL_ERROR "Geos not found [ Geos_DIR:${Geos_DIR} ]")
endif()


# -------------------------------------------------------------------
# Externals
# -------------------------------------------------------------------

option(IgnSocle_WITH_TRIANGLE "Enable Triangle external library" ON)
if(IgnSocle_WITH_TRIANGLE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/externs/triangle-1.6-ign)
    set(IGN_WITH_TRIANGLE ON)
    message(STATUS "Triangle external enabled (built from source)")
else()
    set(IGN_WITH_TRIANGLE OFF)
    message(STATUS "Triangle external disabled")
endif()

#-- recherche de sqlite3 (optionnelle, incluse dans sqlite)
find_package( Sqlite REQUIRED)
if( Sqlite_FOUND )
    # variables pour ign_config.h
    set( IGN_WITH_SQLITE "ON" )
    if (WIN32)
        add_definitions(-D_WIN32_WINNT=0x0501)
    endif()
else()
 #   message( STATUS "building IgnSocle without Sqlite" )
    message( FATAL_ERROR "Sqlite not found" )
endif()


#-- recherche de libpq (optionnelle)
find_package( Pq REQUIRED)

if( Pq_FOUND )
    # variables pour ign_config.h
    set( IGN_WITH_LIBPQ "ON" )
else()
    message( STATUS "building IgnSocle without LIBPQ" )
endif()

#-- recherche de SVM (extension classification)
find_package( SVM QUIET )
if( SVM_FOUND )
    # variables pour ign_config.h
    set( IGN_WITH_SVM "ON" )
else()
    message( STATUS "building IgnSocle without SVM" )
endif()

#-- recherche de OpenCV (extension classification)
OPTION(IgnSocle_WITH_OPENCV "activer/desactiver la dependance a OpenCV" OFF )
set( IGN_WITH_OPENCV OFF )
IF (IgnSocle_WITH_OPENCV)
    find_package(   OpenCV 3.0.0
                    COMPONENTS calib3d core features2d ml
                    NO_DEFAULT_PATH
                    NO_CMAKE_SYSTEM_PATH
                    HINTS "${OpenCV_DIR}"
                )
    if( OpenCV_FOUND )
        # variables pour ign_config.h
        set( IGN_WITH_OPENCV "ON" )
    endif()
ENDIF()

#------------------------------------------------------------------------
# Priorisation librairies statiques / librairies dynamiques
#------------------------------------------------------------------------

set(ALL_POSSIBLE_DEPENDENCIES
    QT
    wxWidgets
    OpenCV
    CGAL
    ECWJ2
    GDAL
    KAKADU
    Geos
      Triangle
    GeoTIFF
    PROJ
    TIFF
    JPEG
    Sqlite3
    PCRE
    GTS
    Glib
    MPFR
    GMP
    OSG
    OOL
    Pq
    PythonInterp
    Shapelib
    SVM
    TinyXML
    Triangle
    Curl
    PNG
    Giflib
    PNG
    ZSTD
    ZLIB
    Boost
    EXPAT
#   ungif
)


set(ALL_AVAILABLE_DEPENDENCIES "")

foreach(POSSIBLE_DEPENDENCY ${ALL_POSSIBLE_DEPENDENCIES})
    if ( ${POSSIBLE_DEPENDENCY}_FOUND)
        LIST (APPEND ALL_AVAILABLE_DEPENDENCIES ${POSSIBLE_DEPENDENCY})
    endif()
endforeach()

if (EXISTS "${SOCLE_EXTERN_ROOT}/CMakeModules/LibraryUtils.cmake")
    include("${SOCLE_EXTERN_ROOT}/CMakeModules/LibraryUtils.cmake")
    message (STATUS "converting the libs to absolute path with BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}")
    foreach(AVAILABLE_DEPENDENCY ${ALL_AVAILABLE_DEPENDENCIES})
        IF (NOT BUILD_SHARED_LIBS)
            SET_ABSOLUTE_LIBRARIES (${AVAILABLE_DEPENDENCY} "STATIC")
        ELSE()
            SET_ABSOLUTE_LIBRARIES (${AVAILABLE_DEPENDENCY} "SHARED")
        ENDIF()
    endforeach(AVAILABLE_DEPENDENCY ${ALL_AVAILABLE_DEPENDENCIES})
endif()

#link and include directories
foreach(AVAILABLE_DEPENDENCY ${ALL_AVAILABLE_DEPENDENCIES})
    include_directories(${${AVAILABLE_DEPENDENCY}_INCLUDE_DIRS})
    link_directories(${${AVAILABLE_DEPENDENCY}_LIBRARY_DIRS})
#    message(STATUS "include_directories [${AVAILABLE_DEPENDENCY}]: ${${AVAILABLE_DEPENDENCY}_INCLUDE_DIRS}")
#    message(STATUS "link_directories [${AVAILABLE_DEPENDENCY}]: ${${AVAILABLE_DEPENDENCY}_LIBRARY_DIRS}")
#    message(STATUS "libraries [${AVAILABLE_DEPENDENCY}]: ${${AVAILABLE_DEPENDENCY}_LIBRARIES}")
endforeach(AVAILABLE_DEPENDENCY ${ALL_AVAILABLE_DEPENDENCIES})

#link directory for current lib
link_directories( ${${PROJECT_NAME}_BINARY_DIR}/lib )


if (IGN_WITH_PROJ)
    set (PROJ_DEPENDENCIES
        PROJ
        Sqlite3
    )
endif()

if (IGN_WITH_GDAL)
    set (GDAL_DEPENDENCIES
        GDAL
    )
endif()

if (IGN_WITH_GEOS)
    set (Geos_DEPENDENCIES
        Geos
    )
endif()
#------------------------------------------------------------------------
# Verification des dependances entre modules pour definir les modules a compiler
#------------------------------------------------------------------------

# 1 NUMERIC
IF(NOT IgnSocle_WITHOUT_NUMERIC)
ENDIF()

# 2 TOOLS
#IF(NOT IgnSocle_WITHOUT_TOOLS)
#ENDIF()

# 3 XML
IF(NOT IgnSocle_WITHOUT_XML)
    IF (NOT TinyXML_FOUND)
        SET(IgnSocle_WITHOUT_XML ON)
        message("-- warning: module XML non compile car dependance TinyXML non trouvee --")
    ENDIF()
ENDIF()

# 4 MATH
IF(NOT IgnSocle_WITHOUT_MATH)
ENDIF()

# 5 DATA
IF(NOT IgnSocle_WITHOUT_DATA)

ENDIF()

# 6 TRANSFORM
IF(NOT IgnSocle_WITHOUT_TRANSFORM)
    IF (IgnSocle_WITHOUT_MATH)
        SET(IgnSocle_WITHOUT_TRANSFORM ON)
        message("-- warning: module TRANSFORM non compile car module MATH desactive --")
    ENDIF()
ENDIF()

# 7 GEODESY
IF(NOT IgnSocle_WITHOUT_GEODESY)
    IF (IgnSocle_WITHOUT_TRANSFORM)
        SET(IgnSocle_WITHOUT_GEODESY ON)
        message("-- warning: module GEODESY non compile car module GEODESY desactive --")
    ENDIF()
ENDIF()

# 8 FILESYSTEM
IF(NOT IgnSocle_WITHOUT_FILESYSTEM)
ENDIF()

# 9 GEOMETRY
IF(NOT IgnSocle_WITHOUT_GEOMETRY)
    IF (NOT Geos_FOUND)
        #SET(IgnSocle_WITHOUT_GEOMETRY ON)
        #message("-- warning: module GEOMETRY non compile car dependance Geos non trouvee --")
        message("-- warning: module GEOMETRY compile sans GEOS. La plupart des algorithmes sont desactives --")
    ENDIF()
    IF(IgnSocle_WITHOUT_DATA)
        SET(IgnSocle_WITHOUT_GEOMETRY ON)
        message("-- warning: module GEOMETRY non compile car module DATA desactive --")
    ENDIF()
    IF (IgnSocle_WITHOUT_TRANSFORM)
        SET(IgnSocle_WITHOUT_GEOMETRY ON)
        message("-- warning: module GEOMETRY non compile car module TRANSFORM desactive --")
    ENDIF()
ENDIF()

# 10 SQL
IF(NOT IgnSocle_WITHOUT_SQL)
    IF(NOT Pq_FOUND)
        SET(IgnSocle_WITHOUT_SQL ON)
        message("-- warning: module SQL non compile car dependance Pq non trouvee --")
    ENDIF()
    IF(IgnSocle_WITHOUT_GEOMETRY)
        SET(IgnSocle_WITHOUT_SQL ON)
        message("-- warning: module SQL non compile car module GEOMETRY desactive --")
    ENDIF()
ENDIF()

# 11 SHAPEFILE
IF(NOT IgnSocle_WITHOUT_SHAPEFILE)
    IF(Shapelib_FOUND)
        set( IgnSocle_WITH_SHAPELIB "ON" )
    ELSE()
        SET(IgnSocle_WITHOUT_SHAPEFILE ON)
        message("-- warning: module SHAPEFILE non compile car dependance Shapelib non trouvee --")
    ENDIF()
ENDIF()

# 12 FEATURE
IF(NOT IgnSocle_WITHOUT_FEATURE)
    IF(IgnSocle_WITHOUT_GEOMETRY)
        SET(IgnSocle_WITHOUT_FEATURE ON)
        message("-- warning: module FEATURE non compile car module GEOMETRY desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_SQL)
        SET(IgnSocle_WITHOUT_FEATURE ON)
        message("-- warning: module FEATURE non compile car module SQL desactive --")
    ENDIF()
    IF(NOT IgnSocle_WITHOUT_FEATURE)
        #-- avec ou sans Shapefile?
        IF(IgnSocle_WITHOUT_SHAPEFILE)
            message("-- warning: module FEATURE compile sans la gestion du SHAPEFILE --")
        ELSE()
            set( IgnSocle_WITH_SHAPELIB "ON" )
        ENDIF()
    ENDIF()
ENDIF()

# 13 GRAPH

# 14 ORIENTATION
IF(NOT IgnSocle_WITHOUT_ORIENTATION)
    IF(IgnSocle_WITHOUT_XML)
        SET(IgnSocle_WITHOUT_ORIENTATION ON)
        message("-- warning: module ORIENTATION non compile car module XML desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_TRANSFORM)
        SET(IgnSocle_WITHOUT_ORIENTATION ON)
        message("-- warning: module ORIENTATION non compile car module TRANSFORM desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_GEODESY)
        SET(IgnSocle_WITHOUT_ORIENTATION ON)
        message("-- warning: module ORIENTATION non compile car module GEODESY desactive --")
    ENDIF()
ENDIF()

# 15 IMAGE
IF(NOT IgnSocle_WITHOUT_IMAGE)
    IF(IgnSocle_WITHOUT_XML)
        SET(IgnSocle_WITHOUT_IMAGE ON)
        message("-- warning: module IMAGE non compile car module XML desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_TOOLS)
        SET(IgnSocle_WITHOUT_IMAGE ON)
        message("-- warning: module IMAGE non compile car module TOOLS desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_DATA)
        SET(IgnSocle_WITHOUT_IMAGE ON)
        message("-- warning: module IMAGE non compile car module DATA desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_TRANSFORM)
        SET(IgnSocle_WITHOUT_IMAGE ON)
        message("-- warning: module IMAGE non compile car module TRANSFORM desactive --")
    ENDIF()

    IF (GDAL_FOUND)
        SET( IgnSocle_WITH_GDAL ON)
    ELSE()
            message("-- warning: module IMAGE compile sans le driver GDAL --")
            SET( IgnSocle_WITH_GDAL OFF)
    endif()

    IF (KAKADU_FOUND)
            # on ne compile plus le driver Kakadu apr defaut
            #SET( IgnSocle_WITH_KAKADU "ON")
    ELSE()
            message(STATUS "-- warning: module IMAGE compile sans le driver KAKADU (mais JP2 supporte si compilation de GDAL avec support JP2) --")
            SET( IgnSocle_WITH_KAKADU "OFF")
    endif()

    #active le format DMR par defaut
    option( IgnSocle_WITH_DMR "build GeoView Image driver DMR" ON )

    IF(IgnSocle_WITH_DMR)
        SET(IGN_WITH_DMR ON)
        IF(WIN32)
            add_definitions("-D__USE_LARGEFILE64")
            add_definitions("-D_LARGEFILE_SOURCE")
            add_definitions("-D_LARGEFILE64_SOURCE")
        ENDIF()
    ELSE()
        SET(IGN_WITH_DMR OFF)
    ENDIF()

#-- activation/desaction de l'extension QT
    IF(QT_FOUND)
        SET(IgnSocle_WITH_QT ON)
    ELSE()
        SET(IgnSocle_WITH_QT OFF)
        message("-- warning: module IMAGE compile sans les dependances a QT --")
    ENDIF()

    IF (NOT IgnSocle_WITHOUT_ORIENTATION)
        SET( IgnSocle_WITH_ORIENTATION "ON")
    ELSE()
        message("-- warning: module IMAGE compile sans l'extension ORIENTATION --")
    endif()

    IF (NOT IgnSocle_WITHOUT_GEOMETRY)
        SET( IGN_DMR_WITH_GEOMETRY "ON")
    ELSE()
        message("-- warning: format DMR du module IMAGE compile sans la gestion de pochoir car module GEOMETRY desactive --")
    endif()
ENDIF()

# 16 LIB3D
IF(NOT IgnSocle_WITHOUT_LIB3D)
    #si on ne compile pas le module orientation, on ne gere pas la lib3d
    IF (IgnSocle_WITHOUT_ORIENTATION)
        SET(IgnSocle_WITHOUT_LIB3D ON)
        message("-- warning: module LIB3D non compile car module orientation desactive --")
    ENDIF()
    #si on ne compile pas le module image, on ne gere pas la lib3d
    IF (IgnSocle_WITHOUT_IMAGE)
        SET(IgnSocle_WITHOUT_LIB3D ON)
        message("-- warning: module LIB3D non compile car  module image desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_GEOMETRY)
        SET(IgnSocle_WITHOUT_LIB3D ON)
        message("-- warning: module LIB3D non compile car module geometry desactive  --")
    ENDIF()
    IF(NOT IGN_WITH_GEOS)
        SET(IgnSocle_WITHOUT_LIB3D ON)
        message("-- warning: module LIB3D non compile car dependance a GEOS desactivee  --")
    ENDIF()
        IF(NOT IGN_WITH_SHAPELIB)
        SET(IgnSocle_WITHOUT_LIB3D ON)
        message(STATUS "-- warning: module LIB3D non compile car dependance a SHAPELIB desactivee  --")
    message (FATAL_ERROR "on interdit temporairement la compilation du socle sans shapelib")
    ENDIF()
ENDIF()

# 17 ALTIMETRY
IF(NOT IgnSocle_WITHOUT_ALTIMETRY)
    #on s'assure que les modules geometry, image et orientation sont compiles ET / OU que le module geometry est compile
    IF(IgnSocle_WITHOUT_GEOMETRY)
        IF (IgnSocle_WITHOUT_IMAGE)
            SET(IgnSocle_WITHOUT_ALTIMETRY ON)
            message("-- warning: module ALTIMETRY non compile car modules GEOMETRY et IMAGE desactives --")
        ENDIF()
        IF (IgnSocle_WITHOUT_ORIENTATION)
            SET(IgnSocle_WITHOUT_ALTIMETRY ON)
            message("-- warning: module ALTIMETRY non compile car  modules GEOMETRY et ORIENTATION desactives --")
        ENDIF()
        IF (NOT IgnSocle_WITHOUT_ORIENTATION)
            message("-- warning: module ALTIMETRY compile sans gestion des MNT TIN car module GEOMETRY desactive --")
            SET (IgnSocle_ALTIMETRY_WITH_RASTER ON)
        ENDIF()
    ELSE()
        SET (IgnSocle_ALTIMETRY_WITH_TIN ON)
        IF (IgnSocle_WITHOUT_IMAGE)
            message("-- warning: module ALTIMETRY compile sans gestion des MNT raster car module IMAGE desactive --")
        ELSE()
            IF (IgnSocle_WITHOUT_ORIENTATION)
                message("-- warning: module ALTIMETRY compile sans gestion des MNT raster car module ORIENTATION desactive --")
            ELSE()
                SET (IgnSocle_ALTIMETRY_WITH_RASTER ON)
            ENDIF()
        ENDIF()

    ENDIF()
ENDIF()

# 18 PHOTOGRAMMETRY
IF(NOT IgnSocle_WITHOUT_PHOTOGRAMMETRY)
    IF (IgnSocle_WITHOUT_GEOMETRY)
        SET(IgnSocle_WITHOUT_PHOTOGRAMMETRY ON)
        message("-- warning: module PHOTOGRAMMETRY non compile car module GEOMETRY desactive --")
    ENDIF()

    #on s'assure que le module altimetry est compile
    IF (IgnSocle_WITHOUT_ALTIMETRY)
        SET(IgnSocle_WITHOUT_PHOTOGRAMMETRY ON)
        message("-- warning: module PHOTOGRAMMETRY non compile car module ALTIMETRY desactive --")
    ENDIF()

    #on s'assure que le module ORIENTATION est compile
    IF (IgnSocle_WITHOUT_ORIENTATION)
        SET(IgnSocle_WITHOUT_PHOTOGRAMMETRY ON)
        message("-- warning: module PHOTOGRAMMETRY non compile car module ORIENTATION desactive --")
    ENDIF()

    #on s'assure que la dependance a Geos est activee
    IF (NOT IGN_WITH_GEOS)
        SET(IgnSocle_WITHOUT_PHOTOGRAMMETRY ON)
        message("-- warning: module PHOTOGRAMMETRY non compile car dependance a GEOS desactivee --")
    ENDIF()
ENDIF()

# 19 RADIOMETRY
IF(NOT IgnSocle_WITHOUT_RADIOMETRY)
    IF (IgnSocle_WITHOUT_GEOMETRY)
        SET(IgnSocle_WITHOUT_RADIOMETRY ON)
        message("-- warning: module RADIOMETRY non compile car module GEOMETRY desactive --")
    ENDIF()

    #on s'assure que le module photogrammetry est compile (ce qui implique que les modules orientation et altimetry le sont egalement)
    IF (IgnSocle_WITHOUT_ALTIMETRY)
        SET(IgnSocle_WITHOUT_RADIOMETRY ON)
        message("-- warning: module RADIOMETRY non compile car module ALTIMETRY desactive --")
    ENDIF()

    #on s'assure que le module image est compile
    IF (IgnSocle_WITHOUT_IMAGE)
        SET(IgnSocle_WITHOUT_RADIOMETRY ON)
        message("-- warning: module RADIOMETRY non compile car  module IMAGE desactive --")
    ENDIF()

ENDIF()

# 20 AERO
IF(NOT IgnSocle_WITHOUT_AERO)
    IF (IgnSocle_WITHOUT_PHOTOGRAMMETRY)
        SET(IgnSocle_WITHOUT_AERO ON)
        message("-- warning: module AERO non compile car module PHOTOGRAMMETRY desactive --")
    ENDIF()
ENDIF()

# 21 GPAO
IF(NOT IgnSocle_WITHOUT_GPAO)
    IF (IgnSocle_WITHOUT_SQL)
        SET(IgnSocle_WITHOUT_GPAO ON)
        message("-- warning: module GPAO non compile car module SQL desactive --")
    ENDIF()
ENDIF()

# 22 LOG
IF(NOT IgnSocle_WITHOUT_LOG)
ENDIF()

#------------------------------------------------------------------------
# Verification des dependances des extensions
#------------------------------------------------------------------------
SET(IgnSocle_PROJECT_EXTENSIONS
    cgal
    umfpack
    gts
    shewchuk
    xtool2
    reco3d
    osg
    interpolation
    ossature
    segmentation
    classification
)

# 1 CGAL
IF(IgnSocle_BUILD_CGAL_EXTENSION)
    IF(IgnSocle_WITHOUT_GRAPH)
        SET(IgnSocle_BUILD_CGAL_EXTENSION OFF)
        message("-- warning: extension CGAL non compilee car module GRAPH desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_GEOMETRY)
        SET(IgnSocle_BUILD_CGAL_EXTENSION OFF)
        message("-- warning: extension CGAL non compilee car module GEOMETRY desactive --")
    ENDIF()
ENDIF()

# 2 umfpack
option( IgnSocle_BUILD_UMFPACK_EXTENSION "build UMFPACK extension" OFF )
IF(IgnSocle_BUILD_UMFPACK_EXTENSION)
ENDIF()


# 3 GTS
IF(IgnSocle_BUILD_GTS_EXTENSION)
    IF(IgnSocle_WITHOUT_GEOMETRY)
        SET(IgnSocle_BUILD_GTS_EXTENSION OFF)
        message("-- warning: extension GTS non compilee car module GEOMETRY desactive --")
    ENDIF()
    
    #-- recherche de GTS (lib3d et extension GTS de geometry)
    find_package( GTS )
    find_package( Glib )
    
    IF (GTS_FOUND AND Glib_FOUND )
        # variables pour ign_config.h
        set( IGN_WITH_GTS "ON" )
    ELSE()
        IF(NOT GTS_FOUND)
            message("-- warning: extension GTS non compilee car la librairie GTS n'est pas trouve --")
        ENDIF()
        IF(NOT Glib_FOUND)
            message("-- warning: extension GTS non compilee car la librairie Glib n'est pas trouve --")
        ENDIF()

        SET(IgnSocle_BUILD_GTS_EXTENSION OFF)

    ENDIF()

ENDIF()

# 4 SHEWCHUK
IF(IgnSocle_BUILD_SHEWCHUK_EXTENSION)
    IF(IgnSocle_WITHOUT_GEOMETRY)
        SET(IgnSocle_BUILD_SHEWCHUK_EXTENSION OFF)
        message("-- warning: extension SHEWCHUK non compilee car module GEOMETRY desactive --")
    ENDIF()

    #    NB 2012/12/18: A priori, shewchuk ne depend pas de Glib???
#    IF(NOT Glib_FOUND)
#        SET(IgnSocle_BUILD_TRIANGLE_EXTENSION OFF)
#    ENDIF()
ENDIF()


# 5 XTOOL2
IF(IgnSocle_BUILD_XTOOL2_EXTENSION)
    IF(IgnSocle_WITHOUT_IMAGE)
        SET(IgnSocle_BUILD_XTOOL2_EXTENSION OFF)
        message("-- warning: extension XTOOL2 non compilee car module IMAGE desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_GEODESY)
        SET(IgnSocle_BUILD_XTOOL2_EXTENSION OFF)
        message("-- warning: extension XTOOL2 non compilee car module GEODESY desactive --")
    ENDIF()
ENDIF()

# 6 RECO3D
IF(IgnSocle_BUILD_RECO3D_EXTENSION)
    IF(IgnSocle_WITHOUT_LIB3D)
        SET(IgnSocle_BUILD_RECO3D_EXTENSION OFF)
        message("-- warning: extension RECO3D non compilee car module LIB3D desactive --")
    ENDIF()
    IF(IgnSocle_WITHOUT_ALTIMETRY)
        message("-- warning: extension RECO3D sera compile sans les algos utilisant l'altimetrie --")
    ENDIF()
    IF(IgnSocle_WITHOUT_ALTIMETRY)
        SET(IgnSocle_BUILD_RECO3D_EXTENSION OFF)
        message("-- warning: extension RECO3D non compilee car module ALTIMETRY desactive --")
    ENDIF()
    IF(NOT IgnSocle_BUILD_CGAL_EXTENSION)
        message("-- warning: tous les modules ne seront pas compiles (recalage) --")
    ENDIF()
ENDIF()

# 7 OSG
IF(IgnSocle_BUILD_OSG_EXTENSION)
    IF(IgnSocle_WITHOUT_LIB3D)
        SET(IgnSocle_BUILD_OSG_EXTENSION OFF)
        message("-- warning: extension OSG non compilee car module LIB3D desactive --")
    ENDIF()
    FIND_PACKAGE(OSG REQUIRED)
    if (NOT OSG_FOUND)
        SET(IgnSocle_BUILD_OSG_EXTENSION OFF)
        message("-- warning: pas de gestion d'export dans lib3d car OSG non trouve ")
    else()
        set(IGN_WITH_OSG "ON")
    endif()
ENDIF()

# 8 interpolation
option( IgnSocle_BUILD_INTERPOLATION_EXTENSION "build interpolation extension" OFF )
IF(NOT IgnSocle_BUILD_UMFPACK_EXTENSION)
    SET(IgnSocle_BUILD_INTERPOLATION_EXTENSION OFF)
    message("-- warning: extension interpolation non compilee car module UMFPACK desactive --")
ENDIF()
IF(NOT IgnSocle_BUILD_CGAL_EXTENSION)
    SET(IgnSocle_BUILD_INTERPOLATION_EXTENSION OFF)
    message("-- warning: extension interpolation non compilee car module CGAL desactive --")
ENDIF()

# 9 Ossature
option( IgnSocle_BUILD_OSSATURE_EXTENSION "build ossature extension" OFF )
IF (IgnSocle_BUILD_OSSATURE_EXTENSION)
    IF (NOT IgnSocle_BUILD_CGAL_EXTENSION)
        SET(IgnSocle_BUILD_OSSATURE_EXTENSION OFF)
        message("-- warning: extension ossature non compilee car module CGAL desactive --")
    ENDIF()
ENDIF()

# 10 Segmentation
option( IgnSocle_BUILD_SEGMENTATION_EXTENSION "build segmentation extension" OFF )
IF (IgnSocle_BUILD_SEGMENTATION_EXTENSION)
    IF(IgnSocle_WITHOUT_IMAGE)
        SET(IgnSocle_BUILD_SEGMENTATION_EXTENSION OFF)
        message("-- warning: extension segmentation non compilee car module IMAGE desactive --")
    ENDIF()
    find_package(wxWidgets COMPONENTS core base)
    if (NOT wxWidgets_FOUND)
        SET(IgnSocle_BUILD_SEGMENTATION_EXTENSION OFF)
        message("-- warning: extension segmentation non compilee car dependace wxWidgets non trouvee --")
    ENDIF()

    IF (APPLE)
        SET(IgnSocle_BUILD_SEGMENTATION_EXTENSION OFF)
        message("-- warning: extension segmentation non compilee car C++11 non active (bug non resolu with type_traits) --")
    ENDIF()
ENDIF()

# 11 Classification
option( IgnSocle_BUILD_CLASSIFICATION_EXTENSION "build classification extension" OFF )
IF (IgnSocle_BUILD_CLASSIFICATION_EXTENSION)
    #on s'assure que la dependance a la libSVM est activee
    IF (NOT SVM_FOUND)
        SET(IgnSocle_BUILD_CLASSIFICATION_EXTENSION OFF)
        message("-- warning: extension classification non compilee car librairie SVM non trouvee --")
    ENDIF()
    #on s'assure que la dependance a la lib OpenCV est activee
    IF (NOT IGN_WITH_OPENCV)
        #SET(IgnSocle_BUILD_CLASSIFICATION_EXTENSION OFF)
        message("-- warning: extension classification compilee sans la dependance a la librairie OpenCV --")
     ENDIF()
ENDIF()

foreach( IgnSocle_PROJECT_MODULE ${IgnSocle_PROJECT_MODULES} )
    STRING(TOUPPER ${IgnSocle_PROJECT_MODULE} ${IgnSocle_PROJECT_MODULE}_UPPER)
    IF(IgnSocle_WITHOUT_${${IgnSocle_PROJECT_MODULE}_UPPER})
        SET(IGN_BUILD_${${IgnSocle_PROJECT_MODULE}_UPPER} 0)
    ELSE()
        SET(IGN_BUILD_${${IgnSocle_PROJECT_MODULE}_UPPER} 1)
    ENDIF()
endforeach()

foreach( IgnSocle_PROJECT_EXTENSION ${IgnSocle_PROJECT_EXTENSIONS} )
    STRING(TOUPPER ${IgnSocle_PROJECT_EXTENSION} ${IgnSocle_PROJECT_EXTENSION}_UPPER)
    IF(IgnSocle_BUILD_${${IgnSocle_PROJECT_EXTENSION}_UPPER}_EXTENSION)
        SET(IGN_BUILD_${${IgnSocle_PROJECT_EXTENSION}_UPPER} 1)
    ELSE()
        SET(IGN_BUILD_${${IgnSocle_PROJECT_EXTENSION}_UPPER} 0)
    ENDIF()
endforeach()

# DEBUG
# IF(IgnSocle_WITHOUT_FEATURE)
#     message(FATAL_ERROR "on interdit temporairement la compilation du socle sans le module 'feature'")
# ENDIF()

#------------------------------------------------------------------------
# Versionning
#------------------------------------------------------------------------
if (EXISTS "${SOCLE_EXTERN_ROOT}/CMakeModules/ParseVersion.cmake")

    include( "${SOCLE_EXTERN_ROOT}/CMakeModules/ParseVersion.cmake" )
    # mb : fix this in order to don't fail build on jenkins & co
    # al : remise pour avoir le num SVN - utile pour la GPAO
    PARSE_VERSION(IgnSocle_WC_REVISION ${CMAKE_SOURCE_DIR})
    #set( IgnSocle_WC_REVISION, 0 )
    MESSAGE(STATUS "-- Current svn / git revision is ${IgnSocle_WC_REVISION}")
endif()

#generation du header de configuration, fonction des bibliotheques disponibles
set( IGN_KEEP_DEPRECATED ON )


message ("IGN_WITH_CGAL: ${IGN_WITH_CGAL}")
message ("IGN_WITH_GEOS: ${IGN_WITH_GEOS}")

configure_file (
  "${CMAKE_SOURCE_DIR}/include/ign/ign_config.h.in"
  "${CMAKE_SOURCE_DIR}/include/ign/ign_config.h"
)

configure_file (
  "${CMAKE_SOURCE_DIR}/include/ign/ign_version.h.in"
  "${CMAKE_SOURCE_DIR}/include/ign/ign_version.h"
)

#------------------------------------------------------------------------
# Configuration des repertoires de sortie
#------------------------------------------------------------------------

#configure le repertoire de sortie de la lib et des binaires
SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
IF(WIN32)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
ELSE(WIN32)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
ENDIF(WIN32)

#------------------------------------------------------------------------
# Compilation des differents modules
#------------------------------------------------------------------------

add_subdirectory( src/ign )

#------------------------------------------------------------------------
# Compilation des extensions
#------------------------------------------------------------------------

add_subdirectory( extension )

#------------------------------------------------------------------------
# Compilation des tests unitaire et de non regression
#------------------------------------------------------------------------

include( CTest )

IF(IgnSocle_BUILD_MODULE_TESTS)
    add_subdirectory( tests/unit )

    IF (IgnSocle_BUILD_MODULE_TOOLS)
        add_subdirectory( tests/tools )
    ENDIF()

    IF (IgnSocle_TEST_COVERAGE)

        find_package(PythonInterp)
        include(CodeCoverage)

        # make coverage permet de creer le rapport en html
    
        # Execute tous les tests (par defaut)
        SETUP_TARGET_FOR_COVERAGE(coverage "ctest --verbose" "testResults")

        # Execute seulement un test (si on est en train de debugger ou de creer ce test)
        # SETUP_TARGET_FOR_COVERAGE(coverage "ctest -R .*image.*" "testResults")

        # make cobertura cree un fichier xml pour le plugin cobertura de jenkins.
        SETUP_TARGET_FOR_COVERAGE_COBERTURA(cobertura ctest "testResultsCobertura" "--verbose")
    ENDIF()
ENDIF()

IF(IgnSocle_BUILD_MODULE_TNR)
    add_subdirectory( tests/regress )
    add_subdirectory( tests/regress )
ENDIF()

#------------------------------------------------------------------------
# Compilation des tools
#------------------------------------------------------------------------

IF(IgnSocle_BUILD_MODULE_TOOLS)
    add_subdirectory( tools )
ENDIF()

#------------------------------------------------------------------------
# Compilation des bindings
#------------------------------------------------------------------------
#add_subdirectory( swig )

#------------------------------------------------------------------------
# Compilation des examples
#------------------------------------------------------------------------
IF(IgnSocle_BUILD_EXAMPLES)
    add_subdirectory( example )
ENDIF()

#------------------------------------------------------------------------
# Generation de la documentation
#------------------------------------------------------------------------
add_subdirectory( doc )

#------------------------------------------------------------------------
# Installation des libraries
#------------------------------------------------------------------------

#-- install directories
install( DIRECTORY include DESTINATION . PATTERN ".svn" EXCLUDE )
install( FILES licence.txt DESTINATION . )
install( FILES IgnSocleConfig.cmake DESTINATION . )
install( FILES IgnSocleConfigVersion.cmake DESTINATION . )
install( FILES CPackConfig.cmake DESTINATION . )
install( DIRECTORY CMakeModules DESTINATION .)
install( DIRECTORY doc DESTINATION .)
install( DIRECTORY example DESTINATION .)
install( DIRECTORY environment DESTINATION .)
install( DIRECTORY data/geodesy DESTINATION data)
install( DIRECTORY data/gpao DESTINATION data)
install( DIRECTORY data/geoportail DESTINATION data)
install( FILES tests/regress/gpao/test_validation_gpao.sh DESTINATION tests/regress/gpao)

#------------------------------------------------------------------------
#-- Configuration de CPack
#------------------------------------------------------------------------
include( CPackConfig.cmake )
#include( CPack )

#------------------------------------------------------------------------
#-- Construction des modules python
#------------------------------------------------------------------------
IF( IgnSocle_BUILD_BOOST_PYTHON_MODULE )
        add_subdirectory( wrapper )
ENDIF()


# This needs to be run very last so other parts of the scripts can take advantage of this.
#IF(NOT IgnSocle_CONFIG_HAS_BEEN_RUN_BEFORE)
#    SET(IgnSocle_CONFIG_HAS_BEEN_RUN_BEFORE 1 CACHE INTERNAL "Flag to track whether this is the first time running CMake or if CMake has been configured before")
#ENDIF()
