/*
 * File:   BoxPacker3D.h
 * Author: arash
 *
 * Created on June 15, 2012, 12:47 PM
 */

#ifndef BOXPACKER3D_H
#define	BOXPACKER3D_H
using namespace std;

class BoxPacker3D : public BoxPacker2D {

public:



    bool packIt( bin_t bin,  item_t item, bin_v_t &bins );


    void splitBinWidth( bin_t bin, item_t item );
    void splitBinHeight( bin_t bin, item_t item );
    void splitBinLength( bin_t bin, item_t item );


protected:

    bool checkFitsNoConstr(bin_t bin,  item_t item, bin_v_t &bins );
    map<string, double> findSubBinSizes( bin_t bin, item_t item);

    bool Fit( bin_t bin,  item_t item, bin_v_t &bins );

    /** Merge unused space
@param[in] packbin  the bin we are packing
@param[in] newbin   the new unused space
@param[in] bins     all the unused bins
@return true if the merge took place
*/
   bool merger( bin_t packbin, bin_t newbin, bin_v_t &bins );

   void Split( bin_t sub_bin, bin_t bin );

};

#endif	/* BOXPACKER3D_H */
/*
 * File:   Item3D.h
 * Author: arash
 *
 * Created on June 14, 2012, 1:17 PM
 */

#ifndef ITEM3D_H
#define	ITEM3D_H
using namespace std;
//#pragma warning( disable : 4250 )

class Item3D : public Shape3D, public Item
{
public:

    static const int NO_CONSTRAINTS = 0;
    static const int CONSTRAINT_WIDTH = 1;
    static const int CONSTRAINT_HEIGHT = 2;
    static const int CONSTRAINT_LENGTH = 3;
    static const int CONSTRAINT_WIDTH_HEIGHT = 4;
    static const int CONSTRAINT_WIDTH_LENGTH = 5;
    static const int CONSTRAINT_HEIGHT_LENGTH = 6;
    Item3D();
    Item3D(const Item3D& orig);
    virtual ~Item3D();

    void Spin( int axis );

    /** Claculate spin axis compared to original orientation
    */

    virtual void SpinAxisCalculate();

    void encodeAsJSON(stringstream &jsonStr);
    string getCSV();


    virtual double getWLocation()
    {
        return myWLocation;
    }
    virtual void setWLocation( double w )
    {
        myWLocation = w;
    }
    virtual double getHLocation()
    {
        return myHLocation;
    }
    virtual void setHLocation( double h )
    {
        myHLocation = h;
    }

    bool IsAboveBelow( std::shared_ptr< Item > other );

private:

};

#endif	/* ITEM3D_H */
/*
 * File:   Shape3D.h
 * Author: arash
 *
 * Created on June 7, 2012, 3:51 PM
 */

#ifndef SHAPE3D_H
#define	SHAPE3D_H

using namespace std;

class Shape3D : virtual public Shape2D
{


protected:

    Side *side_3_;

public:

    Shape3D();
    ~Shape3D();

    Side *side_3();
    void set_side_3( Side *value );

    virtual Side * origSide1();
    virtual Side * origSide2();
    virtual Side * origSide3();
    string origSize();

    virtual void ScaleSize( float f );

    double volume();

    /** Area in width and length
    */
    double AreaWidthLength() const;

    /** Overlap in width and length
    */
    double OverlapArea( std::shared_ptr<Shape> b );

    string getSTL( int offset );

    virtual bool operator <( Shape &b);
    virtual bool operator >( Shape &b);
    virtual bool operator ==( Shape &b);

};

#endif	/* SHAPE3D_H */
#pragma once
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
class Bin;
class Item;
using bin_t = std::shared_ptr< Bin >;
using bin_v_t = std::vector< bin_t >;
using item_t = std::shared_ptr< Item >;
using item_v_t = std::vector< item_t >;
#include "cCut.h"
#include "Side.h"
#include "Shape.h"
#include "Shape2D.h"
#include "Shape3D.h"
#include "Item.h"
#include "Item2D.h"
#include "Item3D.h"

#include "Bin.h"
#include "Bin1D.h"
#include "Bin2D.h"
#include "Bin3D.h"
#include "BoxPacker2D.h"
#include "BoxPacker3D.h"
#include "TubePacker.h"
#include "Bindings.h"
#include "BoxBindingsParser.h"
#include "TubeBindingsParser.h"
#include "Utils.h"



/**  A world of items and bins into which they can be packed
*/
class cPackEngine
{

public:

    /// the bins into which items can be packed
    bin_v_t Bins;

    /// the original bins into which items can be packed, used to reset if pack fails
    bin_v_t BinsOriginal;

    /// the items which can be packed into bins
    item_v_t Items;

    /// the items that could not find a place in a bin
    item_v_t myUnpackedItems;

    /// The dimensionality of the world
    int Dimension;

    /// Construct a new empty world
    cPackEngine()
        : Dimension( 0 )
        , myfRandom( false )
    {
    }
    /**  Build world from input strings

    @param[in] bin_input_description
    @param[in] item_input_description

    @return 0 if no problems building the world
    */
    int Build(
        const char* bin_input_description,
        const char* item_input_description );

    /// Pack items into bins
    void Pack();

    /// JSON format world description and packing results
    string getJson();

    string getCutList();

    /// windex format instructions to display 2D packing arrangement
    string DrawList();

    /// CSV item ids that could not be fittted
    string Unpacked();

    string getCSV();

    /** STL format packing results

    This is a 3D graphics format read by many 3D viewers

    http://www.fabbers.com/tech/STL_Format

    There is a specialized STL-Viewer ( https://github.com/JamesBremner/STL-Viewer )
    which animates the packing when the mousewheel is rotated
    in addition to the usual pan, zoom, etc viewing optiions.

    */
    void getSTL();

    /// Convert feet, cm or m into inches
    static float DimensionUnitScale( const string& unit_string );

    void Clear();

    /// True if user wants all items packed into one bin
    bool myfOneBin;

    /// True if user wants to disable heuristics and do a random pack
    bool myfRandom;

    void PrintAllBins();

    int CountBinsUsed();

    bin_v_t& bins()
    {
        return Bins;
    }

private:
    std::string myError;

    int BuildBins(  vector<string>& bin_v );
    int BuildItems(  vector<string>& item_v );
    bool isDimError( int dim );
    void RemoveBinsTooSmallForAllItems();
    void RemoveSmallestBin();
    void RemoveBinsTooLightForAllItems();
    void RemoveAllButLargestBin();
    bin_t SmallestBinForAllItems();
};

extern cPackEngine thePackEngine;

namespace pack
{
void RemoveUnusedBins( cPackEngine& packer );

}

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include "cPackEngine.h"

using namespace std;

cPackEngine thePackEngine;

bool ParseOriginalOptions( vector< string >& argv,
                           string& bin,
                           string& item,
                           string& shape,
                           string& out_file)
{
    for( int i=1; i < (int)argv.size() ; ++i )
    {
        cout << argv[i] << "\n";

        if( strcmp(argv[i].c_str(), "-b") == 0 )
        {

            bin = argv[i+1];

        }

        if( strcmp(argv[i].c_str(), "-i") == 0 )
        {
            item = argv[i+1];
        }


        if( strcmp(argv[i].c_str(), "-s") == 0 )
        {
            shape = argv[i+1];
        }

        if( strcmp(argv[i].c_str(), "-o") == 0 )
        {
            out_file = argv[i+1];
        }

    }

    if( ! bin.length() )
    {

        cout << "did not get bin in args. pass in using -b" << endl;
        return false;
    }

    if( ! item.length()  )
    {

        cout << "did not get item in args. pass in using -i" << endl;
        return false;
    }


    if( ! shape.length() )
    {

        cout << "did not get bin shape. pass in using -s" << endl;
        return false;

    }
    else if ( shape != "b" && shape != "t" )
    {
        cout << "-s switch can only be used with 'b' or 't'" << endl;
        return false;


    }

    return true;

}


// parse user options

bool ParseNewOptions( int argc, char *argv[],
                      string& bin,
                      string& item,
                      string& shape,
                      string& out_file)
{
//    for( int k = 1; k < argc; k++ )
//        cout << argv[k] << endl;

    namespace po = boost::program_options;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("original",po::value<std::string>(),"Using original command line options\n")
    ("bins", po::value<std::string>(),"bin specification. Comma separate multiple bins. format:\n"
        "{id}:{unit}:{quantity}:{size1}x{size2}x{size3}:{weight}\n"
        "unit can be in, ft, or cm\n"
        "A quantity of -1 indicates an endless supply of bins\n"
        "A weight of 0 indicates ignore weight constraints\n")
    ("items",po::value<std::string>(),"item specification. Comma separate multiple items. format:\n"
        "{id}:{unit}:{constraints}:{quantity}:{size1}x{size2}x{size3}:{weight}\n" )
    ("o", po::value<std::string>(), "output file. the output (json) are written to this file.  prints to stdout if left out")
    ("onebin",po::bool_switch( &thePackEngine.myfOneBin ),"Pack all items into one bin")
    ("random",po::bool_switch( &thePackEngine.myfRandom ),"Disable heuristics, use random pack")
    ;

    // parse the command line
    po::variables_map vm;
    try {

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    }
    catch ( std::exception& e )
    {
        cout <<"\n" << e.what() << endl;
        cout <<"\n" << desc << "\n";
        Utils::displayHelp();
        exit(1);

    }
    if( vm.count("original"))
    {
        vector< string > av;
        boost::split( av,
                      vm["original"].as<std::string>(),
                      boost::is_any_of(" ") );
        return ParseOriginalOptions(
                   av,
                   bin, item, shape, out_file );
    }

    // extract the parameter values
    if (vm.count("help"))
    {
        cout << desc << "\n";
        Utils::displayHelp();
        exit(1);
    }

    if( vm.count("bins") )
    {
        bin = vm["bins"].as<std::string>();
    }
    else
    {
        cout << "did not get bin in args. pass in using --bins" << endl;
        return false;
    }
    if( vm.count("items") )
    {
        item = vm["items"].as<std::string>();
    }
    else
    {
        cout << "did not get items in args. pass in using --items" << endl;
        return false;
    }
    if( vm.count("o") )
    {
        out_file = vm["o"].as<std::string>();
    }



    return true;
}


int main(int argc, char *argv[])
{


    if ( argc == 1 )
    {
        Utils::displayHelp();
        return 0;
    }

    string bin;
    string item;
    string shape;
    string out_file;

    if( ! ParseNewOptions(
                argc, argv,
                bin, item, shape, out_file ) )
        return 1;

    try
    {

        if( thePackEngine.Build( bin.c_str(), item.c_str() ) != 0 )
        {
            throw std::runtime_error( "ERROR: could not build bins and items" );
        }

        thePackEngine.Pack();

    }
    catch ( std::runtime_error& e )
    {
        cout << e.what() << endl;
        if ( out_file.length() )
        {
            fstream file;
            file.open(out_file, fstream::out);
            file << "{\"error\":\"" << e.what() << "\"}" << endl;
            file.close();
        }
        return 1;
    }

    string json_s = thePackEngine.getJson();
    string cutlist_s = thePackEngine.getCutList();
    string drawlist_s = thePackEngine.DrawList();
    string csv = thePackEngine.getCSV();

//    if 	( json_s.find("error") != string::npos )
//    {
//        if( myError != "No bin capable of holding all items" )
//        {
//            cout << json_s << endl;
//            return 1;
//        }
//    }


    if ( ! out_file.length() )
    {
        cout << json_s << endl;
        //cout << cutlist_s;
        cout << drawlist_s;
        cout << csv;
    }
    else
    {

        fstream file;

        file.open(out_file, fstream::out);
        file << json_s;
        file << endl;
        file << cutlist_s;
        file << drawlist_s;
        file.close();

    }

//    ofstream filecsv("packit4me2.csv");
//    filecsv << csv;
//    filecsv.close();


    thePackEngine.getSTL();

    return 0;
}

const char *PACKIT4ME_packThemTube( const char *bins, const char *items)
{

//	TubeBindingsParser *parser = new TubeBindingsParser();
//
//	vector<Bin*> bins_v;
//	vector<Item*> items_v;
//
//	int dim;
//	parser->parseBinsAndItems( bins, items, bins_v, items_v, dim);
//
//	if ( dim == BoxBindingsParser::BIN_PARSE_ERROR )
//	{
//
//		Utils::cleanUpBindingCall( bins_v, items_v, parser );
//		return "error, could not parse bin.  please confirm bin string format id:v1[xv2xv3]";
//	}
//
//	if ( dim == BoxBindingsParser::ITEM_PARSE_ERROR )
//	{
//
//		Utils::cleanUpBindingCall( bins_v, items_v, parser );
//		return "error, could not parse item.  please confirm item string format id:constraints:v1[xv2xv3]";
//	}
//
//	if ( dim == BoxBindingsParser::PACKER_ERROR )
//	{
//
//		Utils::cleanUpBindingCall( bins_v, items_v, parser );
//		return "error, check input strings";
//
//	}
//
//	TubePacker *packer = new TubePacker();
//	packer->packThem(bins_v, items_v);
//	delete packer;
//
//
//	stringstream jsonStr;
//	jsonStr << "[";
//	for( unsigned i=0; i < bins_v.size(); ++i )
//	{
//		bins_v[i]->encodeAsJSON(jsonStr, false);
//		if ( i != bins_v.size() - 1)
//			jsonStr << ",";
//	}
//
//	jsonStr << "]";
//
//
//
//	string json = jsonStr.str();
//
//	char *s_buffer =  new char[json.size()+1];
//	copy(json.begin(), json.end(), s_buffer);
//	s_buffer[json.size()] = '\0';
//
//	Utils::cleanUpBindingCall( bins_v, items_v, parser );
//
//
//	return s_buffer;
    return "error, tube packing not implemented";
}

#include "cPackEngine.h"

cPackEngine thePackEngine;

int main( int argc, char* argv[] )
{
    thePackEngine.Build(
        "b1:in:-1:10x10:100",
        "i1:in:7:200:1x1:1" );        // spin disallowed
    thePackEngine.Pack();

    std::cout << "Bins used: " << thePackEngine.CountBinsUsed() << "\n";

    return 0;
}
