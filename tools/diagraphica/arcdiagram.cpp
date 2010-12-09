// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./arcdiagram.cpp

#include "wx.hpp" // precompiled headers

#include <limits>
#include "arcdiagram.h"
#include <iostream>

using namespace std;


// -- init static variables -----------------------------------------


// general
//ColorRGB ArcDiagram::colClr = { 1.0, 1.0, 0.93, 1.0 };
ColorRGB ArcDiagram::colClr = { 1.0, 1.0, 1.0, 1.0 };
ColorRGB ArcDiagram::colTxt = { 0.0, 0.0, 0.0, 1.0 };
int ArcDiagram::szeTxt = 12;
// cluster tree
bool ArcDiagram::showTree = true;
bool ArcDiagram::annotateTree = true;
int ArcDiagram::colorMap = VisUtils::COL_MAP_QUAL_SET_3;
// bar tree
bool ArcDiagram::showBarTree = true;
double ArcDiagram::magnBarTree = 0.0;
// arc diagram
bool ArcDiagram::showLeaves = true;
bool ArcDiagram::showBundles = true;
ColorRGB ArcDiagram::colBundles = { 0.0, 0.0, 0.0, 0.3 };
int ArcDiagram::itvAnim = 100;


// -- init constants ------------------------------------------------


int ArcDiagram::MIN_RAD_HINT_PX =  3;
int ArcDiagram::MAX_RAD_HINT_PX = 30;
int ArcDiagram::SEGM_HINT_HQ    = 24;
int ArcDiagram::SEGM_HINT_LQ    = 12;


// -- constructors and destructor -----------------------------------


// ------------------------
ArcDiagram::ArcDiagram(
    Mediator* m,
    Graph* g,
    GLCanvas* c )
    : Visualizer( m, g, c )
// ------------------------
{
    idxInitStLeaves    = -1;

    diagram = NULL;

    timerAnim = new wxTimer();
    timerAnim->SetOwner( this, ID_TIMER );
}


// ----------------------
ArcDiagram::~ArcDiagram()
// ----------------------
{
    diagram = NULL;
    clearSettings();

    delete timerAnim;
    timerAnim = NULL;
}


// -- get functions -------------------------------------------------


// -------------------------------
ColorRGB ArcDiagram::getColorClr()
// -------------------------------
{
    return colClr;
}


// -------------------------------
ColorRGB ArcDiagram::getColorTxt()
// -------------------------------
{
    return colTxt;
}


// -------------------------
int ArcDiagram::getSizeTxt()
// -------------------------
{
    return szeTxt;
}


// -------------------------------
double ArcDiagram::getIntervAnim()
// -------------------------------
{
    return itvAnim;
}


// ---------------------------
bool ArcDiagram::getShowTree()
// ---------------------------
{
    return showTree;
}


// -------------------------------
bool ArcDiagram::getAnnotateTree()
// -------------------------------
{
    return annotateTree;
}


// --------------------------
int ArcDiagram::getColorMap()
// --------------------------
{
    return colorMap;
}


// ------------------------------
bool ArcDiagram::getShowBarTree()
// ------------------------------
{
    return showBarTree;
}


// --------------------------------
double ArcDiagram::getMagnBarTree()
// --------------------------------
{
    return magnBarTree;
}


// -----------------------------
bool ArcDiagram::getShowLeaves()
// -----------------------------
{
    return showLeaves;
}


// ------------------------------
bool ArcDiagram::getShowBundles()
// ------------------------------
{
    return showBundles;
}


// -----------------------------------
ColorRGB ArcDiagram::getColorBundles()
// -----------------------------------
{
    return colBundles;
}


// --------------------------------
double ArcDiagram::getTrspBundles()
// --------------------------------
{
    return colBundles.a;
}


// -------------------------------------------------
void ArcDiagram::getAttrsTree( vector< int > &idcs )
// -------------------------------------------------
{
    idcs.clear();
    for ( size_t i = 0; i < attrsTree.size(); ++i )
        idcs.push_back( attrsTree[i]->getIndex() );
}


// -- set functions -------------------------------------------------


// ------------------------------------------------
void ArcDiagram::setColorClr( const ColorRGB &col )
// ------------------------------------------------
{
    colClr = col;
}


// ------------------------------------------------
void ArcDiagram::setColorTxt( const ColorRGB &col )
// ------------------------------------------------
{
    colTxt = col;
}


// ------------------------------------------
void ArcDiagram::setSizeTxt( const int &sze )
// ------------------------------------------
{
    szeTxt = sze;
}


// ---------------------------------------------
void ArcDiagram::setIntervAnim( const int &itv )
// ---------------------------------------------
{
    itvAnim = itv;
}

// --------------------------------------------
void ArcDiagram::setShowTree( const bool &shw )
// --------------------------------------------
{
    showTree = shw;
}


// ------------------------------------------------
void ArcDiagram::setAnnotateTree( const bool &shw )
// ------------------------------------------------
{
    annotateTree = shw;
}


// ----------------------------------------------
void ArcDiagram::setColorMap( const int &colMap )
// ----------------------------------------------
{
    colorMap = colMap;
}


// -----------------------------------------------
void ArcDiagram::setShowBarTree( const bool &shw )
// -----------------------------------------------
{
    showBarTree = shw;
}


// -------------------------------------------------
void ArcDiagram::setMagnBarTree( const double &val )
// -------------------------------------------------
{
    magnBarTree = val;
}


// ----------------------------------------------
void ArcDiagram::setShowLeaves( const bool &shw )
// ----------------------------------------------
{
    showLeaves = shw;
}


// -----------------------------------------------
void ArcDiagram::setShowBundles( const bool &shw )
// -----------------------------------------------
{
    showBundles = shw;
}


// ----------------------------------------------------
void ArcDiagram::setColorBundles( const ColorRGB &col )
// ----------------------------------------------------
{
    colBundles = col;
}


// --------------------------------------------------
void ArcDiagram::setTrspBundles( const double &trsp )
// --------------------------------------------------
{
    colBundles.a = trsp;
}


// ------------------------------------------------------
void ArcDiagram::setAttrsTree( const vector< int > idcs )
// ------------------------------------------------------
{
    attrsTree.clear();
    for ( size_t i = 0; i < idcs.size(); ++i )
        attrsTree.push_back( graph->getAttribute( idcs[i] ) );
}


// -----------------------------------------
void ArcDiagram::setDiagram( Diagram *dgrm )
// -----------------------------------------
{
    diagram = dgrm;
}


// -------------------------------
void ArcDiagram::hideAllDiagrams()
// -------------------------------
{
    {
    for ( size_t i = 0; i < showDgrm.size(); ++i )
        showDgrm[i] = false;
    }

    {
    for ( size_t i = 0; i < markBundles.size(); ++i )
        markBundles[i] = false;
    }
}


// -----------------------
void ArcDiagram::markLeaf(
    const int &leafIdx,
    ColorRGB &col )
// -----------------------
{
    map< int, vector< ColorRGB > >::iterator it;
    it = markLeaves.find( leafIdx );
    if ( it != markLeaves.end() )
        it->second.push_back( col );
    else
    {
        vector< ColorRGB > v;
        v.push_back( col );
        markLeaves.insert( pair< int, vector< ColorRGB > >( leafIdx, v ) );
    }
}


// ----------------------------
void ArcDiagram::unmarkLeaves()
// ----------------------------
{
    markLeaves.clear();
}


// ------------------------------------------
void ArcDiagram::markBundle( const size_t &idx )
// ------------------------------------------
{
    if ( idx < markBundles.size() )
        markBundles[idx] = true;
}


// -----------------------------
void ArcDiagram::unmarkBundles()
// -----------------------------
{
    for ( size_t i = 0; i < markBundles.size(); ++i )
        markBundles[i] = false;
}


// ---------------------------------------
void ArcDiagram::handleSendDgrmSglToSiml()
// ---------------------------------------
{
    mediator->initSimulator(
        framesDgrm[currIdxDgrm][frameIdxDgrm[currIdxDgrm]],
        attrsDgrm[currIdxDgrm] );
}


// ----------------------------------------
void ArcDiagram::handleSendDgrmSglToTrace()
// ----------------------------------------
{
    mediator->markTimeSeries( this, framesDgrm[currIdxDgrm][frameIdxDgrm[currIdxDgrm]] );
}


// ----------------------------------------
void ArcDiagram::handleSendDgrmSetToTrace()
// ----------------------------------------
{
    mediator->markTimeSeries( this, framesDgrm[currIdxDgrm] );
}


// ---------------------------------------
void ArcDiagram::handleSendDgrmSglToExnr()
// ---------------------------------------
{
    mediator->addToExaminer(
        framesDgrm[currIdxDgrm][frameIdxDgrm[currIdxDgrm]],
        attrsDgrm[currIdxDgrm] );
}


// ---------------------------------------
void ArcDiagram::handleSendDgrmSetToExnr()
// ---------------------------------------
{
    mediator->addToExaminer(
        framesDgrm[currIdxDgrm],
        attrsDgrm[currIdxDgrm] );
}


// -- visualization functions  --------------------------------------


// ---------------------------------------------------

void ArcDiagram::visualize( const bool &inSelectMode )
// ---------------------------------------------------
{
    // have textures been generated
    if ( texCharOK != true )
        genCharTex();

    // check if positions are ok
    if ( geomChanged == true )
        calcSettingsGeomBased();
    if ( dataChanged == true )
        calcSettingsDataBased();

    // selection mode
    if ( inSelectMode == true )
    {
        double wth, hgt;
        canvas->getSize( wth, hgt );

        GLint hits = 0;
        GLuint selectBuf[512];
        startSelectMode(
            hits,
            selectBuf,
            2.0,
            2.0 );

        glPushName( ID_CANVAS );
        VisUtils::fillRect( -0.5*wth, 0.5*wth, 0.5*hgt, -0.5*hgt );

        if ( showTree == true )
        {
            if ( annotateTree == true )
                drawTreeLvls( inSelectMode );

            drawTree( inSelectMode );
        }
        if ( showBarTree == true )
            drawBarTree( inSelectMode );
        if ( showBundles == true )
            drawBundles( inSelectMode );
        if ( showLeaves == true )
        {
            drawLeaves( inSelectMode );
            drawDiagrams( inSelectMode );
        }
        glPopName();

        finishSelectMode(
            hits,
            selectBuf );
    }
    // rendering mode
    else
    {
        clear();
        if ( showTree == true )
        {
            if ( annotateTree == true )
                drawTreeLvls( inSelectMode );

            drawTree( inSelectMode );
        }
        if ( showBarTree == true )
            drawBarTree( inSelectMode );
        if ( showBundles == true )
            drawBundles( inSelectMode );
        if ( showLeaves == true )
        {
            drawLeaves( inSelectMode );
            drawMarkedLeaves( inSelectMode );
        }
        drawDiagrams( inSelectMode );
    }
}


// -----------------------------------------------------
void ArcDiagram::drawBundles( const bool &inSelectMode )
// -----------------------------------------------------
{
    // selection mode
    if ( inSelectMode == true )
    {
        int segs = SEGM_HINT_HQ;

        glPushName( ID_BUNDLES );
        for ( size_t i = 0; i < posBundles.size(); ++i )
        {
            double x      = posBundles[i].x;
            double y      = posBundles[i].y;
            double rad    = radiusBundles[i];
            double orient = orientBundles[i];
            double wth    = widthBundles[i];

            glPushName((GLuint) i );
            if ( orient < 0 )
            {
                VisUtils::fillArc( x, y, 180.0, 0.0, wth, 0.0, rad, segs );
                VisUtils::drawArc( x, y, 180.0, 0.0, wth, 0.0, rad, segs );
            }
            else if ( orient > 0 )
            {
                VisUtils::fillArc( x, y, 0.0, 180.0, wth, 0.0, rad, segs );
                VisUtils::drawArc( x, y, 0.0, 180.0, wth, 0.0, rad, segs );
            }
            else
            {
                VisUtils::fillArc( x, y, 180.0, 540.0, wth, 0.0, rad, 2*segs );
                VisUtils::drawArc( x, y, 180.0, 540.0, wth, 0.0, rad, 2*segs );
            }
            glPopName();
        }
        glPopName();
    }
    // rendering mode
    else
    {
        // render high quality
        if ( mouseDrag == MSE_DRAG_FALSE )
        {
            ColorRGB colFill;
            ColorRGB colFade;
            ColorRGB colBrdrFill;
            ColorRGB colBrdrFade;

            /*
            colFill       = colBundles;
            */
            colFade       = colClr;
            /*
            colFade.a     = colFill.a;
            colBrdrFill   = colBundles;
            colBrdrFill.a = 1.2*colFill.a;
            colBrdrFade   = colBundles;
            colBrdrFade.a = 0.1*colFill.a;
            */

            int segs = SEGM_HINT_HQ;

            VisUtils::enableLineAntiAlias();
            VisUtils::enableBlending();

            for ( size_t i = 0; i < posBundles.size(); ++i )
            {
                if ( markBundles[i] == true )
                    VisUtils::mapColorDkCoolBlue( colFill );
                else
                    colFill = colBundles;
                colFade.a     = colFill.a;
                colBrdrFill   = colFill;
                colBrdrFill.a = 1.2*colFill.a;
                colBrdrFade   = colFill;
                colBrdrFade.a = 0.1*colFill.a;

                double x      = posBundles[i].x;
                double y      = posBundles[i].y;
                double rad    = radiusBundles[i];
                double orient = orientBundles[i];
                double wth    = widthBundles[i];

                if ( orient < 0 )
                {
                    VisUtils::fillArc( x, y, 180.0, 0.0, wth, 0.0, colFill,     colFade,     rad, segs );
                    VisUtils::drawArc( x, y, 180.0, 0.0, wth, 0.0, colBrdrFill, colBrdrFade, rad, segs );
                }
                else if ( orient > 0 )
                {
                    VisUtils::fillArc( x, y, 0.0, 180.0, wth, 0.0, colFill,     colFade,     rad, segs );
                    VisUtils::drawArc( x, y, 0.0, 180.0, wth, 0.0, colBrdrFill, colBrdrFade, rad, segs );
                }
                else
                {
                    VisUtils::fillArc( x, y, 180.0, 540.0, wth, 0.0, colFill,     colFade,     rad, 2*segs );
                    VisUtils::drawArc( x, y, 180.0, 540.0, wth, 0.0, colBrdrFill, colBrdrFade, rad, 2*segs );
                }
            }

            VisUtils::disableLineAntiAlias();
            VisUtils::disableBlending();
        }
        // render low quality
        else
        {
            VisUtils::setColorLtGray();

            int segs = SEGM_HINT_HQ;

            for ( size_t i = 0; i < posBundles.size(); ++i )
            {
                double x      = posBundles[i].x;
                double y      = posBundles[i].y;
                double rad    = radiusBundles[i];
                double orient = orientBundles[i];

                if ( orient < 0 )
                    VisUtils::drawArc( x, y, 180.0, 0.0, rad, segs );
                else if ( orient > 0 )
                    VisUtils::drawArc( x, y, 0.0, 180.0, rad, segs );
                else
                    VisUtils::drawArc( x, y, 180.0, 540.0, rad, 2*segs );
            }
        }
    }
}


// ----------------------------------------------------
void ArcDiagram::drawLeaves( const bool &inSelectMode )
// ----------------------------------------------------
{
    // selection mode
    if ( inSelectMode == true )
    {
        glPushName( ID_LEAF_NODE );
        for ( size_t i = 0; i < posLeaves.size(); ++i )
        {
            double x = posLeaves[i].x;
            double y = posLeaves[i].y;

            glPushName((GLuint) i );
            VisUtils::fillEllipse( x, y, radLeaves, radLeaves, 24 );
            glPopName();
        }
        glPopName();
    }
    // rendering mode
    else
    {
        // render high quality
        if ( mouseDrag == MSE_DRAG_FALSE )
        {
            int segs = SEGM_HINT_HQ;
            Cluster* clust = NULL;
            ColorRGB colFill;

            VisUtils::enableLineAntiAlias();
            for ( size_t i = 0; i < posLeaves.size(); ++i )
            {
                // position
                double x = posLeaves[i].x;
                double y = posLeaves[i].y;

                // color
                //clust   = mapPosToClust[posTreeTopLft.size()-1][i];
                clust = graph->getLeaf( i );

                if ( clust != NULL && clust->getAttribute() != NULL )
                    calcColor(
                        clust->getAttrValIdx(),
                        clust->getAttribute()->getSizeCurValues()-1,
                        colFill );
                else
                    VisUtils::mapColorWhite( colFill );

                // drop shadow
                VisUtils::setColorMdGray();
                VisUtils::drawEllipse( x+0.2*radLeaves, y-0.2*radLeaves, radLeaves, radLeaves, segs );
                VisUtils::fillEllipse( x+0.2*radLeaves, y-0.2*radLeaves, radLeaves, radLeaves, segs );

                // foreground
                VisUtils::setColor( colFill );
                VisUtils::fillEllipse( x, y, radLeaves, radLeaves, segs );
                VisUtils::setColorDkGray();
                VisUtils::drawEllipse( x, y, radLeaves, radLeaves, segs );
            }

            // mark cluster with initial state
            if ( idxInitStLeaves >= 0 )
            {
                double x = posLeaves[idxInitStLeaves].x;
                double y = posLeaves[idxInitStLeaves].y;

                VisUtils::setColorLtGray();
                VisUtils::fillEllipse( x, y, 0.5*radLeaves, 0.5*radLeaves, segs );
                VisUtils::setColorMdGray();
                VisUtils::drawEllipse( x, y, 0.5*radLeaves, 0.5*radLeaves, segs );
            }

            VisUtils::disableLineAntiAlias();

            clust = NULL;
        }
        // render low quality
        else
        {
            int segs = SEGM_HINT_LQ;

            for ( size_t i = 0; i < posLeaves.size(); ++i )
            {
                double x = posLeaves[i].x;
                double y = posLeaves[i].y;

                VisUtils::setColorWhite();
                VisUtils::fillEllipse( x, y, radLeaves, radLeaves, segs );

                VisUtils::setColorDkGray();
                VisUtils::drawEllipse( x, y, radLeaves, radLeaves, segs );
            }
        }
    }
}


// --------------------------------------------------
void ArcDiagram::drawTree( const bool &inSelectMode )
// --------------------------------------------------
{
    // selection mode
    if ( inSelectMode == true )
    {
        glPushName( ID_TREE_NODE );
        for ( size_t i = 0; i < posTreeTopLft.size()-1; ++i )
        {
            glPushName((GLuint) i );
            for ( size_t j = 0; j < posTreeTopLft[i].size(); ++j )
            {
                double xLft = posTreeTopLft[i][j].x;
                double xRgt = posTreeBotRgt[i][j].x;
                double yTop = posTreeTopLft[i][j].y;

                glPushName((GLuint) j );
                VisUtils::fillEllipse( 0.5*(xLft+xRgt), yTop, 0.75*radLeaves, 0.75*radLeaves, 24 );
                glPopName();
            }
            glPopName();
        }
        glPopName();
    }
    // rendering mode
    else
    {
        // render high quality
        if ( mouseDrag == MSE_DRAG_FALSE )
        {
            int segs = SEGM_HINT_HQ;

            Cluster* clust = NULL;
            ColorRGB colFill;

            ColorRGB colFade;
            VisUtils::mapColorLtLtGray( colFade );

            VisUtils::enableLineAntiAlias();
            VisUtils::enableBlending();
            for ( size_t i = 0; i < posTreeTopLft.size()-1; ++i )
            {
                for ( size_t j = 0; j < posTreeTopLft[i].size(); ++j )
                {
                    double xLft = posTreeTopLft[i][j].x;
                    double xRgt = posTreeBotRgt[i][j].x;
                    double yTop = posTreeTopLft[i][j].y;
                    double yBot = posTreeBotRgt[i][j].y;

                    // color
                    clust   = mapPosToClust[i][j];
                    if ( clust != NULL && clust->getAttribute() != NULL )
                    {
                        calcColor(
                            clust->getAttrValIdx(),
                            clust->getAttribute()->getSizeCurValues()-1,
                            colFill );
                    }
                    else
                        VisUtils::mapColorLtGray( colFill );

                    // triangle
                    VisUtils::fillTriangle(
                        0.5*(xLft+xRgt), yTop, colFill,
                        xLft,            yBot, colFade,
                        xRgt,            yBot, colFade );
                    VisUtils::setColorLtGray();
                    VisUtils::drawTriangle( 0.5*(xLft+xRgt), yTop, xLft, yBot, xRgt, yBot );

                    // drop shadow
                    VisUtils::setColorMdGray();
                    VisUtils::drawEllipse( 0.5*(xLft+xRgt)+0.1*radLeaves, yTop-0.1*radLeaves, 0.75*radLeaves, 0.75*radLeaves, segs );
                    VisUtils::setColorMdGray();
                    VisUtils::fillEllipse( 0.5*(xLft+xRgt)+0.1*radLeaves, yTop-0.1*radLeaves, 0.75*radLeaves, 0.75*radLeaves, segs );

                    // foreground
                    colFill.a = 1.0;
                    VisUtils::setColor( colFill );
                    VisUtils::fillEllipse( 0.5*(xLft+xRgt), yTop, 0.75*radLeaves, 0.75*radLeaves, segs );
                    VisUtils::setColorDkGray();
                    VisUtils::drawEllipse( 0.5*(xLft+xRgt), yTop, 0.75*radLeaves, 0.75*radLeaves, segs );
                }
            }
            VisUtils::disableBlending();
            VisUtils::disableLineAntiAlias();
        }
        // render low quality
        else
        {
            int segs = SEGM_HINT_LQ;

            for ( size_t i = 0; i < posTreeTopLft.size()-1; ++i )
            {
                for ( size_t j = 0; j < posTreeTopLft[i].size(); ++j )
                {
                    double xLft = posTreeTopLft[i][j].x;
                    double xRgt = posTreeBotRgt[i][j].x;
                    double yTop = posTreeTopLft[i][j].y;
                    double yBot = posTreeBotRgt[i][j].y;

                    VisUtils::setColorLtLtGray();
                    VisUtils::fillTriangle(0.5*(xLft+xRgt), yTop, xLft, yBot, xRgt, yBot );
                    VisUtils::setColorLtGray();
                    VisUtils::drawTriangle(0.5*(xLft+xRgt), yTop, xLft, yBot, xRgt, yBot );

                    VisUtils::setColorWhite();
                    VisUtils::fillEllipse( 0.5*(xLft+xRgt), yTop, 0.75*radLeaves, 0.75*radLeaves, segs );
                    VisUtils::setColorDkGray();
                    VisUtils::drawEllipse( 0.5*(xLft+xRgt), yTop, 0.75*radLeaves, 0.75*radLeaves, segs );
                }
            }
        }
    }
}


// ------------------------------------------------------
void ArcDiagram::drawTreeLvls( const bool &inSelectMode )
// ------------------------------------------------------
{
    double wth = canvas->getWidth();
    double pix = canvas->getPixelSize();

    // selection mode
    if ( inSelectMode == true )
    {}
    // rendering mode
    else
    {
        // render high quality
        if ( mouseDrag == MSE_DRAG_FALSE )
        {
            string lbl;

            ColorRGB colLine;
            VisUtils::mapColorLtGray( colLine );

            ColorRGB colText = colTxt;

			for ( size_t i = 0; i < posTreeTopLft.size()-1; ++i )
            {
                if ( posTreeTopLft[i].size() > 0 )
                {
                    lbl = mapPosToClust[i+1][0]->getAttribute()->getName();

                    double yLin =  posTreeBotRgt[i][0].y;
                    double yTxt =  yLin + 0.5*szeTxt*pix + pix;

                    // left
                    double xLft = -0.5*wth + radLeaves;
                    double xRgt =  posTreeTopLft[i][0].x - 2.0*radLeaves;

                    VisUtils::setColor( colText );
                    VisUtils::drawLabelRight( texCharId, xLft, yTxt, szeTxt*pix/CHARHEIGHT, lbl );
                    VisUtils::setColor( colLine );
                    VisUtils::drawLine( xLft, xRgt, yLin, yLin );

                    // right
                    xLft = posTreeBotRgt[i][posTreeBotRgt[i].size()-1].x + 2.0*radLeaves;
                    xRgt = 0.5*wth - radLeaves;

                    VisUtils::setColor( colText );
                    VisUtils::drawLabelLeft( texCharId, xRgt, yTxt, szeTxt*pix/CHARHEIGHT, lbl );
                    VisUtils::setColor( colLine );
                    VisUtils::drawLine( xLft, xRgt, yLin, yLin );
                }
            }
        }
    }
}


// -----------------------------------------------------
void ArcDiagram::drawBarTree( const bool &inSelectMode )
// -----------------------------------------------------
{
    // selection mode
    if ( inSelectMode == true )
    {
        if ( posBarTreeTopLft.size() > 1 )
        {
            glPushName( ID_BAR_TREE );
            for ( size_t i = 0; i < posBarTreeTopLft.size(); ++i )
            {
                glPushName((GLuint) i );
                for ( size_t j = 0; j < posBarTreeTopLft[i].size(); ++j )
                {
                    double xLft = posBarTreeTopLft[i][j].x;
                    double xRgt = posBarTreeBotRgt[i][j].x;
                    double yTop = posBarTreeTopLft[i][j].y;
                    double yBot = posBarTreeBotRgt[i][j].y;

                    glPushName((GLuint) j );
                    VisUtils::fillRect( xLft, xRgt, yTop, yBot );
                    glPopName();
                }
                glPopName();
            }
            glPopName();
        }
    }
    // rendering mode
    else
    {
        if ( posBarTreeTopLft.size() > 1 )
        {
            // render high quality
            if ( mouseDrag == MSE_DRAG_FALSE )
            {
                Cluster* clust = NULL;;
                ColorRGB colFill;
                ColorRGB colFade;
                ColorRGB colBrdr;

                // fade & border color
                VisUtils::mapColorLtLtGray( colFade );
                VisUtils::mapColorLtGray( colBrdr );

                //colFade.a   *= 0.15;

                VisUtils::enableLineAntiAlias();
                VisUtils::enableBlending();

                for ( size_t i = 0; i < posBarTreeTopLft.size(); ++i )
                {
                    for ( size_t j = 0; j < posBarTreeTopLft[i].size(); ++j )
                    {
                        // position
                        double xLft = posBarTreeTopLft[i][j].x;
                        double xRgt = posBarTreeBotRgt[i][j].x;
                        double yTop = posBarTreeTopLft[i][j].y;
                        double yBot = posBarTreeBotRgt[i][j].y;

                        // fill color
                        clust   = mapPosToClust[i][j];
                        if ( clust != NULL && clust->getAttribute() != NULL )
                            calcColor(
                                clust->getAttrValIdx(),
                                clust->getAttribute()->getSizeCurValues()-1,
                                colFill );
                        else
                            VisUtils::mapColorLtGray( colFill );

                        // solid background
                        VisUtils::setColor( colClr );
                        VisUtils::fillRect( xLft, xRgt, yTop, yBot );

                        // colored foreground
                        VisUtils::fillRect(
                            xLft,    xRgt,
                            yTop,    yBot,
                            colFill, colFade,
                            colFill, colFade );

                        // border
                        VisUtils::setColor( colBrdr );
                        VisUtils::drawRect( xLft, xRgt, yTop, yBot );
                    }
                }
                VisUtils::disableBlending();
                VisUtils::disableLineAntiAlias();

                clust = NULL;
            }
            // render low quality
            else
            {
                for ( size_t i = 0; i < posBarTreeTopLft.size(); ++i )
                {
                    for ( size_t j = 0; j < posBarTreeTopLft[i].size(); ++j )
                    {
                        double xLft = posBarTreeTopLft[i][j].x;
                        double xRgt = posBarTreeBotRgt[i][j].x;
                        double yTop = posBarTreeTopLft[i][j].y;
                        double yBot = posBarTreeBotRgt[i][j].y;

                        VisUtils::setColorLtLtGray();
                        VisUtils::fillRect( xLft, xRgt, yTop, yBot );
                        VisUtils::setColorLtGray();
                        VisUtils::drawRect( xLft, xRgt, yTop, yBot );
                    }
                }
            }
        }
    }
}


// ------------------------------------------------------
void ArcDiagram::drawDiagrams( const bool &inSelectMode )
// ------------------------------------------------------
{
    // selection mode
    if ( inSelectMode == true )
    {
        glPushName( ID_DIAGRAM );
        for ( size_t i = 0; i < posDgrm.size(); ++i )
        {
            if ( showDgrm[i] == true )
            {
                double x   = posDgrm[i].x;
                double y   = posDgrm[i].y;

                glPushName((GLuint) i );
                glPushMatrix();
                glTranslatef( x, y, 0.0 );
                glScalef( 0.2f, 0.2f, 0.2f );

                vector< double > vals;
                /*
                for ( int j = 0; j < attrsDgrm[i].size(); ++j )
                    vals.push_back(
                        attrsDgrm[i][j]->mapToValue(
                            framesDgrm[i][frameIdxDgrm[i]]->getNode(0)->getTupleVal(
                                attrsDgrm[i][j]->getIndex() ) )->getIndex() );
                */
                Attribute* attr;
                Node* node;
                for ( size_t j = 0; j < attrsDgrm[i].size(); ++j )
                {
                    attr = attrsDgrm[i][j];
                    node = framesDgrm[i][frameIdxDgrm[i]]->getNode(0);
                    if ( attr->getSizeCurValues() > 0 )
                        vals.push_back( attr->mapToValue( node->getTupleVal( attr->getIndex() ) )->getIndex() );
                    else
                    {
                        double val = node->getTupleVal( attr->getIndex() );
                        vals.push_back( val );
                    }
                }
                attr = NULL;
                node = NULL;

                diagram->visualize(
                    inSelectMode,
                    canvas,
                    attrsDgrm[i],
                    vals );
                vals.clear();

                // close
                glPushName( ID_DIAGRAM_CLSE );
                VisUtils::fillRect( 0.8, 0.96, 0.96, 0.8 );
                glPopName();

                glPushName( ID_DIAGRAM_MORE );
                VisUtils::fillRect( -0.98, -0.8, -0.8, -0.98 );
                glPopName();

                if ( framesDgrm[i].size() > 1 )
                {
                    // rewind
                    glPushName( ID_DIAGRAM_RWND );
                    VisUtils::fillRect( 0.2, 0.36, -0.8, -0.98 );
                    glPopName();
                    // prevous
                    glPushName( ID_DIAGRAM_PREV );
                    VisUtils::fillRect( 0.4, 0.56, -0.8, -0.98 );
                    glPopName();
                    // play/pause
                    glPushName( ID_DIAGRAM_PLAY );
                    VisUtils::fillRect( 0.6, 0.76, -0.8, -0.98 );
                    glPopName();
                    // next
                    glPushName( ID_DIAGRAM_NEXT );
                    VisUtils::fillRect( 0.8, 0.96, -0.8, -0.98 );
                    glPopName();
                }

                glPopMatrix();
                glPopName();
            }
        }
        glPopName();
    }
    // rendering mode
    else
    {
        string msg;

        for ( size_t i = 0; i < posDgrm.size(); ++i )
        {
            if ( showDgrm[i] == true )
            {
                double xL = posLeaves[i].x;
                double yL = posLeaves[i].y;
                double xD = posDgrm[i].x;
                double yD = posDgrm[i].y;
                double aglDeg = Utils::calcAngleDg( xD-xL, yD-yL );
                double dist   = Utils::dist( xL, yL, xD, yD );
                double pix    = canvas->getPixelSize();

                glPushMatrix();
                if ( mouseDrag == MSE_DRAG_FALSE )
                {
                    if ( i == currIdxDgrm)
                        VisUtils::setColorCoolBlue();
                    else
                        VisUtils::setColorMdGray();

                    glPushMatrix();
                    glTranslatef( xL, yL, 0.0 );
                    glRotatef( aglDeg-90.0, 0.0, 0.0, 1.0 );
                    VisUtils::enableLineAntiAlias();
                    VisUtils::fillTriangle( 0.0, 0.0, -pix, dist, pix, dist );
                    VisUtils::drawTriangle( 0.0, 0.0, -pix, dist, pix, dist );
                    VisUtils::fillEllipse( 0.0, 0.0, 0.25*radLeaves, 0.25*radLeaves, 24 );
                    VisUtils::drawEllipse( 0.0, 0.0, 0.25*radLeaves, 0.25*radLeaves, 24 );
                    VisUtils::disableLineAntiAlias();
                    glPopMatrix();
                }
                else
                {
                    VisUtils::setColorMdGray();
                    VisUtils::drawLine( xL, xD, yL, yD );
                }

                glTranslatef( xD, yD, 0.0 );
                if ( mouseDrag == MSE_DRAG_FALSE )
                {
                    if ( i == currIdxDgrm )
                        VisUtils::fillRect( -0.2+4.0*pix, 0.2+4.0*pix, 0.2-4.0*pix, -0.2-4.0*pix );
                    else
                        VisUtils::fillRect( -0.2+3.0*pix, 0.2+3.0*pix, 0.2-3.0*pix, -0.2-3.0*pix );
                }
                glScalef( 0.2f, 0.2f, 0.2f );

                if ( i == animIdxDgrm )
                {
                    vector< double > vals;
                    /*
                    for ( int j = 0; j < attrsDgrm[i].size(); ++j )
                        vals.push_back(
                            attrsDgrm[i][j]->mapToValue(
                                framesDgrm[i][frameIdxDgrm[i]]->getNode(0)->getTupleVal(
                                    attrsDgrm[i][j]->getIndex() ) )->getIndex() );
                    */
                    Attribute* attr;
                    Node* node;
                    for ( size_t j = 0; j < attrsDgrm[i].size(); ++j )
                    {
                        attr = attrsDgrm[i][j];
                        node = framesDgrm[i][frameIdxDgrm[i]]->getNode(0);
                        if ( attr->getSizeCurValues() > 0 )
                            vals.push_back( attr->mapToValue( node->getTupleVal( attr->getIndex() ) )->getIndex() );
                        else
                        {
                            double val = node->getTupleVal( attr->getIndex() );
                            vals.push_back( val );
                        }
                    }
                    attr = NULL;
                    node = NULL;

                    diagram->visualize(
                        inSelectMode,
                        canvas,
                        attrsDgrm[i],
                        vals );
                    vals.clear();
                }
                else
                {
                    vector< double > vals;
                    /*
                    for ( int j = 0; j < attrsDgrm[i].size(); ++j )
                        vals.push_back(
                            attrsDgrm[i][j]->mapToValue(
                                framesDgrm[i][frameIdxDgrm[i]]->getNode(0)->getTupleVal(
                                    attrsDgrm[i][j]->getIndex() ) )->getIndex() );
                    */
                    Attribute* attr;
                    Node* node;
                    for ( size_t j = 0; j < attrsDgrm[i].size(); ++j )
                    {
                        attr = attrsDgrm[i][j];
                        node = framesDgrm[i][frameIdxDgrm[i]]->getNode(0);
                        if ( attr->getSizeCurValues() > 0 )
                            vals.push_back( attr->mapToValue( node->getTupleVal( attr->getIndex() ) )->getIndex() );
                        else
                        {
                            double val = node->getTupleVal( attr->getIndex() );
                            vals.push_back( val );
                        }
                    }
                    attr = NULL;
                    node = NULL;

                    diagram->visualize(
                        inSelectMode,
                        canvas,
                        attrsDgrm[i],
                        vals );
                    vals.clear();
                }

                msg  = "";
                msg += Utils::dblToStr( frameIdxDgrm[i]+1 );
                msg += "/";
                msg += Utils::dblToStr( framesDgrm[i].size() );

                VisUtils::setColor( colTxt );
                VisUtils::drawLabelRight( texCharId, -0.76, -0.89, 5*szeTxt*pix/CHARHEIGHT, msg );

                VisUtils::enableLineAntiAlias();

                if ( i == currIdxDgrm)
                    VisUtils::setColorCoolBlue();
                else
                    VisUtils::setColorMdGray();
                VisUtils::fillCloseIcon( 0.8, 0.96, 0.96, 0.8 );
                VisUtils::setColorLtLtGray();
                VisUtils::drawCloseIcon( 0.8, 0.96, 0.96, 0.8 );

                if ( i == currIdxDgrm)
                    VisUtils::setColorCoolBlue();
                else
                    VisUtils::setColorMdGray();
                VisUtils::fillMoreIcon( -0.98, -0.8, -0.8, -0.98 );
                VisUtils::setColorLtLtGray();
                VisUtils::drawMoreIcon( -0.98, -0.8, -0.8, -0.98 );

                if ( framesDgrm[i].size() > 1 )
                {
                    if ( i == currIdxDgrm)
                        VisUtils::setColorCoolBlue();
                    else
                        VisUtils::setColorMdGray();
                    VisUtils::fillRwndIcon( 0.2, 0.36, -0.8, -0.98 );
                    VisUtils::setColorLtLtGray();
                    VisUtils::drawRwndIcon( 0.2, 0.36, -0.8, -0.98 );

                    if ( i == currIdxDgrm)
                        VisUtils::setColorCoolBlue();
                    else
                        VisUtils::setColorMdGray();
                    VisUtils::fillPrevIcon( 0.4, 0.56, -0.8, -0.98 );
                    VisUtils::setColorLtLtGray();
                    VisUtils::drawPrevIcon( 0.4, 0.56, -0.8, -0.98 );

                    if ( timerAnim->IsRunning() &&
                         animIdxDgrm == i )
                    {
                        if ( i == currIdxDgrm)
                            VisUtils::setColorCoolBlue();
                        else
                            VisUtils::setColorMdGray();
                        VisUtils::fillPauseIcon( 0.6, 0.76, -0.8, -0.98 );
                        VisUtils::setColorLtLtGray();
                        VisUtils::drawPauseIcon( 0.6, 0.76, -0.8, -0.98 );
                    }
                    else
                    {
                        if ( i == currIdxDgrm)
                            VisUtils::setColorCoolBlue();
                        else
                            VisUtils::setColorMdGray();
                        VisUtils::fillPlayIcon( 0.6, 0.76, -0.8, -0.98 );
                        VisUtils::setColorLtLtGray();
                        VisUtils::drawPlayIcon( 0.6, 0.76, -0.8, -0.98 );
                    }

                    if ( i == currIdxDgrm)
                        VisUtils::setColorCoolBlue();
                    else
                        VisUtils::setColorMdGray();
                    VisUtils::fillNextIcon( 0.8, 0.96, -0.8, -0.98 );
                    VisUtils::setColorLtLtGray();
                    VisUtils::drawNextIcon( 0.8, 0.96, -0.8, -0.98 );
                }
                VisUtils::disableLineAntiAlias();
                glPopMatrix();
            }
        }
    }
}


// ----------------------------------------------------------
void ArcDiagram::drawMarkedLeaves( const bool &inSelectMode )
// ----------------------------------------------------------
{
    // selection mode
    if ( inSelectMode == true )
    {}
    // rendering mode
    else
    {
        if ( mouseDrag == MSE_DRAG_FALSE && markLeaves.size() > 0 )
        {
            double pix  = canvas->getPixelSize();
            int segs = SEGM_HINT_HQ;

            for ( size_t i = 0; i < posLeaves.size(); ++i )
            {
                map< int, vector< ColorRGB > >::iterator it;
                it = markLeaves.find((int) i );

                if ( it != markLeaves.end() )
                {
                    double x = posLeaves[i].x;
                    double y = posLeaves[i].y;

                    double frac = 1.0/(double)it->second.size();
                    for ( size_t j = 0; j < it->second.size(); ++j )
                    {
                        double aglBeg = j*frac*360.0;
                        double aglEnd = (j+1)*frac*360.0;

                        ColorRGB colIn, colOut;
                        colIn = it->second[j];
                        colOut = colIn;
                        colOut.a = 0.0;

                        VisUtils::enableLineAntiAlias();
                        VisUtils::setColor( colIn );
                        VisUtils::drawArc(
                            x, y,
                            aglBeg, aglEnd,
                            radLeaves+pix, segs );
                        VisUtils::fillEllipse(
                            x, y,
                            radLeaves+pix, radLeaves+pix,
                            radLeaves+15*pix, radLeaves+15*pix,
                            aglBeg, aglEnd,
                            segs,
                            colIn, colOut );
                        VisUtils::disableLineAntiAlias();

                    }
                }
            }
        }

    }
}


// -- input event handlers ------------------------------------------


// --------------------------------------
void ArcDiagram::handleMouseLftDownEvent(
    const int &x,
    const int &y )
// --------------------------------------
{
    Visualizer::handleMouseLftDownEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// ------------------------------------
void ArcDiagram::handleMouseLftUpEvent(
    const int &x,
    const int &y )
// ------------------------------------
{
    Visualizer::handleMouseLftUpEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );

    dragIdxDgrm = -1;
}


// ----------------------------------------
void ArcDiagram::handleMouseLftDClickEvent(
    const int &x,
    const int &y )
// ----------------------------------------
{
    Visualizer::handleMouseLftDClickEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// --------------------------------------
void ArcDiagram::handleMouseRgtDownEvent(
    const int &x,
    const int &y )
// --------------------------------------
{
    Visualizer::handleMouseRgtDownEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// ------------------------------------
void ArcDiagram::handleMouseRgtUpEvent(
    const int &x,
    const int &y )
// ------------------------------------
{
    Visualizer::handleMouseRgtUpEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );
}


// -------------------------------------
void ArcDiagram::handleMouseMotionEvent(
    const int &x,
    const int &y )
// -------------------------------------
{
    Visualizer::handleMouseMotionEvent( x, y );

    // redraw in select mode
    visualize( true );
    // redraw in render mode
    visualize( false );

    if ( showMenu != true )
        handleDragDiagram();
    else
        showMenu = false;

    xMousePrev = xMouseCur;
    yMousePrev = yMouseCur;
}


// ---------------------------------
void ArcDiagram::updateDiagramData()
// ---------------------------------
{
    for ( size_t i = 0; i < attrsDgrm.size(); ++i )
    {
        // diagram is showing
        if ( showDgrm[i] == true )
        {
            // get previous position
            Position2D pos = posDgrm[i];
            // re-initiate diagram
            showDiagram( i );
            // update position to previous
            posDgrm[i] = pos;
        }
    }
}


// -- utility drawing functions -------------------------------------


// ---------------------
void ArcDiagram::clear()
// ---------------------
{
    VisUtils::clear( colClr );
}


// ------------------------
void ArcDiagram::calcColor(
    const int &iter,
    const int &numr,
    ColorRGB &col )
// ------------------------
{
    if ( colorMap == VisUtils::COL_MAP_QUAL_PAST_1 )
        VisUtils::mapColorQualPast1(
            iter,
            numr,
            col );
    else if ( colorMap == VisUtils::COL_MAP_QUAL_PAST_2 )
        VisUtils::mapColorQualPast2(
            iter,
            numr,
            col );
    else if ( colorMap == VisUtils::COL_MAP_QUAL_SET_1 )
        VisUtils::mapColorQualSet1(
            iter,
            numr,
            col );
    else if ( colorMap == VisUtils::COL_MAP_QUAL_SET_2 )
        VisUtils::mapColorQualSet2(
            iter,
            numr,
            col );
    else if ( colorMap == VisUtils::COL_MAP_QUAL_SET_3 )
        VisUtils::mapColorQualSet3(
            iter,
            numr,
            col );
    else if ( colorMap == VisUtils::COL_MAP_QUAL_PAIR )
        VisUtils::mapColorQualPair(
            iter,
            numr,
            col );
    else if ( colorMap == VisUtils::COL_MAP_QUAL_DARK )
        VisUtils::mapColorQualDark(
            iter,
            numr,
            col );
    else if ( colorMap == VisUtils::COL_MAP_QUAL_ACCENT )
        VisUtils::mapColorQualAccent(
            iter,
            numr,
            col );
}


// -------------------------------------
void ArcDiagram::calcSettingsGeomBased()
// -------------------------------------
{
    // update flag
    geomChanged = false;

    calcSettingsLeaves();
    calcSettingsBundles();
    calcSettingsTree();
    calcSettingsBarTree();
}


// -------------------------------------
void ArcDiagram::calcSettingsDataBased()
// -------------------------------------
{
    // update flag
    dataChanged = false;

    calcSettingsDiagram();
}


// ----------------------------------
void ArcDiagram::calcSettingsLeaves()
// ----------------------------------
{
    if ( graph->getSizeLeaves() > 0 )
    {
        // get size of canvas & 1 pixel
        double w, h;
        canvas->getSize( w, h );
        double pix = canvas->getPixelSize();

        // calc lft & rgt boundary
        double xLft = -0.5*Utils::minn( w, h )+20*pix;
        double xRgt =  0.5*Utils::minn( w, h )-20*pix;

        // get number of values on x-axis
        double numX = graph->getSizeLeaves();

        // calc intervals per axis
        double fracX;
        if ( numX > 1 )
            fracX = ( 1.0/(double)(numX) )*( xRgt-xLft );
        else
            fracX = 1.0*( xRgt-xLft );

        // calc radius
        radLeaves = 0.15*fracX;
        if ( radLeaves < MIN_RAD_HINT_PX*pix )
            radLeaves = MIN_RAD_HINT_PX*pix;
        else if ( radLeaves >= MAX_RAD_HINT_PX*pix )
            radLeaves = MAX_RAD_HINT_PX*pix;

        // clear prev positions
        posLeaves.clear();
        // calc positions
        {
        for ( int i = 0; i < numX; ++i )
        {
            double x = xLft + 0.5*fracX + i*fracX;
            double y = 0.0;
            Position2D pos;
            pos.x = x;
            pos.y = y;
            posLeaves.push_back( pos );
        }
        }

        // update idx of init state
        idxInitStLeaves = graph->getNode(0)->getCluster()->getIndex();
    }

    prevFrameIdxClust = -1;
    currFrameIdxClust = -1;
    nextFrameIdxClust = -1;
}


// -----------------------------------
void ArcDiagram::calcSettingsBundles()
// -----------------------------------
{
    if ( graph->getSizeBundles() > 0 )
    {
        // clear prev settings
        posBundles.clear();
        radiusBundles.clear();
        orientBundles.clear();
        widthBundles.clear();
        markBundles.clear();

        // max size
        double maxSize = 0;
        {
        for ( int i = 0; i < graph->getSizeBundles(); ++i )
            if ( graph->getBundle(i)->getSizeEdges() > maxSize )
                maxSize = graph->getBundle(i)->getSizeEdges();
        }

        // calc new settings
        {
        for ( int i = 0; i < graph->getSizeBundles(); ++i )
        {
            int idxFr = graph->getBundle(i)->getInCluster()->getIndex();
            int idxTo = graph->getBundle(i)->getOutCluster()->getIndex();
            Position2D pos;

            // position
            if ( idxFr == idxTo )
            {
                pos.x = posLeaves[idxFr].x + radLeaves;
                pos.y = posLeaves[idxFr].y;
            }
            else if ( idxFr < idxTo )
            {
                pos.x = 0.5*(posLeaves[idxFr].x + posLeaves[idxTo].x);
                pos.y = 0.5*(posLeaves[idxFr].y + posLeaves[idxFr].y);
            }
            else if ( idxFr > idxTo )
            {
                pos.x = 0.5*(posLeaves[idxFr].x + posLeaves[idxTo].x);
                pos.y = 0.5*(posLeaves[idxFr].y + posLeaves[idxFr].y);
            }
            posBundles.push_back( pos );

            // radius
            double rad;
            if ( idxFr == idxTo )
                rad = radLeaves;
            else
                rad = 0.5*Utils::abs( Utils::dist(
                    posLeaves[idxFr].x,  posLeaves[idxFr].y,
                    posLeaves[idxTo].x , posLeaves[idxTo].y ) );
            radiusBundles.push_back( rad );

            // width
            double frac = (double)graph->getBundle(i)->getSizeEdges()/maxSize;
            rad = sqrt( frac*(2.0*radLeaves)*(2.0*radLeaves) );
            widthBundles.push_back( rad );

            // orientation
            if ( idxFr < idxTo )
                orientBundles.push_back( 1 );
            else if ( idxFr > idxTo )
                orientBundles.push_back( -1 );
            else
                orientBundles.push_back( 0 );

            markBundles.push_back( false );
            //updateMarkBundles();
        }
        }
    }
}


// --------------------------------
void ArcDiagram::calcSettingsTree()
// --------------------------------
{
    if ( graph->getRoot() != NULL )
    {
        // get size of canvas & 1 pixel
        double w, h;
        canvas->getSize( w, h );

        // calc lft & rgt boundary
        double yTop = 0.5*Utils::minn( w, h )-2.0*radLeaves;

        // clear prev settings
        clearSettingsTree();

        // calc max depth of clustering tree
        size_t maxLvl = 0;
        /*
        {
        for ( int i = 0; i < graph->getSizeLeaves(); ++i )
        {
            if ( graph->getLeaf(i)->getSizeCoord() > maxLvl )
                maxLvl = graph->getLeaf(i)->getSizeCoord();
        }
        }
        */
        maxLvl = attrsTree.size() + 1;

        // init positions
        {
        for ( size_t i = 0; i < maxLvl; ++i )
        {
            vector< Position2D > p;
            posTreeTopLft.push_back( p );
            posTreeBotRgt.push_back( p );

            vector< Cluster* > c;
            mapPosToClust.push_back( c );
        }
        }

        // calc positions
        calcPositionsTree( graph->getRoot(), maxLvl, yTop/(double)(maxLvl-1) );
    }
}


// --------------------------------
void ArcDiagram::calcPositionsTree(
    Cluster* c,
    const int &maxLvl,
    const double &itvHgt )
// --------------------------------
{
    for ( int i = 0; i < c->getSizeChildren(); ++i )
        calcPositionsTree( c->getChild(i), maxLvl, itvHgt );

    Position2D topLft;
    Position2D botRgt;
    int        lvl = c->getSizeCoord()-1;

    vector< int > v;
    c->getCoord( v );

    if ( c->getSizeChildren() != 0 )
    {
        int    numChildren = c->getSizeChildren();

        topLft.x = 0.5*( posTreeTopLft[lvl+1][posTreeTopLft[lvl+1].size()-numChildren].x
                      +  posTreeBotRgt[lvl+1][posTreeBotRgt[lvl+1].size()-numChildren].x );
        botRgt.x = 0.5*( posTreeTopLft[lvl+1][posTreeTopLft[lvl+1].size()-1].x
                      +  posTreeBotRgt[lvl+1][posTreeBotRgt[lvl+1].size()-1].x );

        /*
        {
        for ( int i = 0; i < v.size(); ++i )
        {
            *mediator << v[i];
            *mediator << " ";
        }
        }
        *mediator << " -> ";

        *mediator << lvl;
        *mediator << "\n";
        */

        topLft.y = (((maxLvl-1)-  lvl  )*itvHgt);
        botRgt.y = (((maxLvl-1)-(lvl+1))*itvHgt);
    }
    else
    {
        topLft.x = posLeaves[c->getIndex()].x;
        botRgt.x = posLeaves[c->getIndex()].x;
        /*
        topLft.y = posLeaves[c->getIndex()].y + radLeaves;
        botRgt.y = posLeaves[c->getIndex()].y;
        */

        topLft.y = (((maxLvl-1)-  lvl  )*itvHgt);
        botRgt.y = posLeaves[c->getIndex()].y;
    }

    posTreeTopLft[lvl].push_back( topLft );
    posTreeBotRgt[lvl].push_back( botRgt );
    mapPosToClust[lvl].push_back( c );
}


// -----------------------------------
void ArcDiagram::calcSettingsBarTree()
// -----------------------------------
{
    if ( graph->getRoot() != NULL )
    {
        // get size of canvas & 1 pixel
        double w, h;
        canvas->getSize( w, h );

        // calc lft & rgt boundary
        double yBot = -0.5*Utils::minn( w, h );
        double hght = abs( yBot )-2.0*radLeaves;

        // clear prev settings
        clearSettingsBarTree();

        // calc max depth of clustering tree
        int maxLvl = 0;
        {
        for ( int i = 0; i < graph->getSizeLeaves(); ++i )
        {
            if ( graph->getLeaf(i)->getSizeCoord() > maxLvl )
                maxLvl = graph->getLeaf(i)->getSizeCoord();
        }
        }

        // init positions
        {
        for ( int i = 0; i < maxLvl; ++i )
        {
            vector< Position2D > p;
            posBarTreeTopLft.push_back( p );
            posBarTreeBotRgt.push_back( p );
        }
        }

        // calc positions
        calcPositionsBarTree(
            graph->getRoot(), yBot, hght );
    }
}


// -----------------------------------
void ArcDiagram::calcPositionsBarTree(
    Cluster* c,
    const double &yBot,
    const double &height )
// -----------------------------------
{
    for ( int i = 0; i < c->getSizeChildren(); ++i )
        calcPositionsBarTree( c->getChild(i), yBot, height );

    Position2D topLft;
    Position2D botRgt;
    int        lvl = c->getSizeCoord()-1;

    if ( c->getSizeChildren() != 0 )
    {
        int    numChildren = c->getSizeChildren();

        topLft.x = 0.5*( posBarTreeTopLft[lvl+1][posBarTreeTopLft[lvl+1].size()-numChildren].x
                      +  posBarTreeBotRgt[lvl+1][posBarTreeBotRgt[lvl+1].size()-numChildren].x );
        botRgt.x = 0.5*( posBarTreeTopLft[lvl+1][posBarTreeTopLft[lvl+1].size()-1].x
                      +  posBarTreeBotRgt[lvl+1][posBarTreeBotRgt[lvl+1].size()-1].x );

        double frac = (double)c->getSizeDescNodes()/(double)graph->getSizeNodes();

        topLft.y = yBot + Utils::fishEye( magnBarTree, frac )*height;
        botRgt.y = yBot;
    }
    else
    {
        topLft.x = posLeaves[c->getIndex()].x - radLeaves;
        botRgt.x = posLeaves[c->getIndex()].x + radLeaves;

        double frac = (double)c->getSizeDescNodes()/(double)graph->getSizeNodes();

        topLft.y = yBot + Utils::fishEye( magnBarTree, frac )*height;
        botRgt.y = yBot;
    }

    posBarTreeTopLft[lvl].push_back( topLft );
    posBarTreeBotRgt[lvl].push_back( botRgt );
}


// -----------------------------------
void ArcDiagram::calcSettingsDiagram()
// -----------------------------------
{
    clearSettingsDiagram();
    for ( size_t i = 0; i < posLeaves.size(); ++i )
    {
        showDgrm.push_back( false );

        vector< Attribute* > va;
        attrsDgrm.push_back( va );

        vector< Cluster* > vc;
        framesDgrm.push_back( vc );
        frameIdxDgrm.push_back( 0 );

        Position2D p;
        p.x = 0;
        p.y = 0;
        posDgrm.push_back( p );
    }
    dragIdxDgrm      = -1;
    animIdxDgrm      = -1;
    currIdxDgrm      = -1;
}


// ---------------------------------
void ArcDiagram::updateMarkBundles()
// ---------------------------------
{
    for ( size_t i = 0; i < markBundles.size(); ++i )
        markBundles[i] = false;

    if ( currIdxDgrm != std::numeric_limits< size_t >::max() )
    {
        Cluster* clst;
        Node* node;
        Edge* edge = NULL;

        clst = framesDgrm[currIdxDgrm][frameIdxDgrm[currIdxDgrm]];
        for ( int j = 0; j < clst->getSizeNodes(); ++j )
        {
            node = clst->getNode( j );
            {
            for ( int k = 0; k < node->getSizeInEdges(); ++k )
            {
                edge = node->getInEdge( k );
				if( edge != NULL &&
					0 <= edge->getBundle()->getIndex() &&
					static_cast<int>(markBundles.size()) > edge->getBundle()->getIndex() )
				{
					markBundles[edge->getBundle()->getIndex()] = true;
				}
            }

            }
            {
            for ( int k = 0; k < node->getSizeOutEdges(); ++k )
            {
                edge = node->getOutEdge( k );
				if( edge != NULL &&
					0 <= edge->getBundle()->getIndex() &&
					static_cast<int>(markBundles.size()) > edge->getBundle()->getIndex() )
				{
					markBundles[edge->getBundle()->getIndex()] = true;
				}
            }
            }
        }

        edge = NULL;
        node = NULL;
        clst = NULL;
    }
}


// -----------------------------
void ArcDiagram::clearSettings()
// -----------------------------
{
    clearSettingsBundles();
    clearSettingsLeaves();
    clearSettingsTree();
    clearSettingsDiagram();

    attrsTree.clear();
}


// -----------------------------------
void ArcDiagram::clearSettingsLeaves()
// -----------------------------------
{
    posLeaves.clear();
    idxInitStLeaves = -1;
}


// ------------------------------------
void ArcDiagram::clearSettingsBundles()
// ------------------------------------
{
    posBundles.clear();
    radiusBundles.clear();
    widthBundles.clear();
    orientBundles.clear();
}


// ---------------------------------
void ArcDiagram::clearSettingsTree()
// ---------------------------------
{
    for ( size_t i = 0; i < posTreeTopLft.size(); ++i )
    {
        posTreeTopLft[i].clear();
        posTreeBotRgt[i].clear();
        mapPosToClust[i].clear();
    }

    posTreeTopLft.clear();
    posTreeBotRgt.clear();
    mapPosToClust.clear();
}


// ------------------------------------
void ArcDiagram::clearSettingsBarTree()
// ------------------------------------
{
    for ( size_t i = 0; i < posBarTreeTopLft.size(); ++i )
    {
        posBarTreeTopLft[i].clear();
        posBarTreeBotRgt[i].clear();
    }

    posBarTreeTopLft.clear();
    posBarTreeBotRgt.clear();
}


// ------------------------------------
void ArcDiagram::clearSettingsDiagram()
// ------------------------------------
{
    showDgrm.clear();

    {
    for ( size_t i = 0; i < attrsDgrm.size(); ++i )
        attrsDgrm[i].clear();
    }
    attrsDgrm.clear();

    {
    for ( size_t i = 0; i < framesDgrm.size(); ++i )
    {
        for( size_t j = 0; j < framesDgrm[i].size(); ++j )
            delete framesDgrm[i][j];
        framesDgrm[i].clear();
    }
    }
    framesDgrm.clear();
    frameIdxDgrm.clear();

    posDgrm.clear();
}


// -- utility event handlers ------------------------------------


// ----------------------------------------
void ArcDiagram::onTimer( wxTimerEvent &/*e*/ )
// ----------------------------------------
{
    if ( timerAnim->GetInterval() != itvAnim )
    {
        timerAnim->Stop();
        timerAnim->Start( itvAnim );
    }

    frameIdxDgrm[animIdxDgrm] += 1;
    if ( static_cast <size_t> (frameIdxDgrm[animIdxDgrm]) >= framesDgrm[animIdxDgrm].size() )
        frameIdxDgrm[animIdxDgrm] = 0;
    updateMarkBundles();

    visualize( false );
    canvas->Refresh();
}


// ----------------------------------------------------
void ArcDiagram::handleHits( const vector< int > &ids )
// ----------------------------------------------------
{
    if ( mouseButton == MSE_BUTTON_DOWN )
    {
        if ( mouseDrag == MSE_DRAG_TRUE )
        {
        	if ( mouseSide == MSE_SIDE_LFT )
            {
            	if ( ids[1] == ID_DIAGRAM )
            	{
                	handleDragDiagram(ids[2]);
                }
            }
       }
    }
    else // mouse button up
    {
    	if ( ids.size() == 1 )  // leaves
    	{
        	if ( currIdxDgrm != std::numeric_limits< size_t >::max() )
        	{
			currIdxDgrm = -1;
            		updateMarkBundles();
            		mediator->handleUnshowFrame();
        	}
        	canvas->clearToolTip();
    	}
    	else
    	{
        	// interact with bundles
        	if ( ids[1] == ID_BUNDLES )
        	{
            		currIdxDgrm = -1;
           		handleHoverBundle( ids[2] );
        	}
        	// interact with tree nodes
        	else if ( ids[1] == ID_TREE_NODE )
        	{
            		currIdxDgrm = -1;
            		updateMarkBundles();
            		mediator->handleUnshowFrame();

            		if ( mouseButton == MSE_BUTTON_DOWN &&
                		mouseDrag == MSE_DRAG_FALSE )
            		{
                		if ( mouseSide == MSE_SIDE_LFT )
                		{
				/*
				*mediator << "expand or collapse\n";
				*/

				//Cluster* hitClust = mapPosToClust[ids[2]][ids[3]];
				//vector< int > v;
				//hitClust->getCoord( v );

				//graph->clearSubClusters( v );
				//geomChanged = true;
				}
				/*
				else if ( mouseSide == MSE_SIDE_RGT )
				{
				Cluster* hitClust = mapPosToClust[ids[2]][ids[3]];
				mediator->handleEditClust( hitClust );
				hitClust = NULL;
				}
				*/
            		}
            		else
            		{
                		handleHoverCluster( ids[2], ids[3] );
            		}
        	}
        	// interact with leaf nodes
        	else if ( ids[1] == ID_LEAF_NODE )
        	{
            		if ( mouseClick == MSE_CLICK_SINGLE &&
                 		mouseDrag == MSE_DRAG_FALSE &&
                 		mouseSide == MSE_SIDE_LFT )
            		{
            			handleShowDiagram( ids[2] );
                		if ( mediator->getView() == Mediator::VIEW_TRACE )
                		{
                    			mediator->markTimeSeries( this, graph->getLeaf( ids[2] ) );
                		}
            		}
			else if( mouseClick == MSE_CLICK_SINGLE &&
				 mouseDrag == MSE_DRAG_FALSE &&
				 mouseSide == MSE_SIDE_RGT )
			{
				/*mediator->handleShowClusterMenu();*/ // Select attributes from the popup menu for clustering.
			}
            		else
            		{
                		currIdxDgrm = -1;
                		updateMarkBundles();
                		mediator->handleUnshowFrame();

                		handleHoverCluster( mapPosToClust.size()-1, ids[2] );
            		}
        	}
        	// interact with bar tree
        	else if ( ids[1] == ID_BAR_TREE )
        	{
			currIdxDgrm = -1;
			updateMarkBundles();
			mediator->handleUnshowFrame();

			handleHoverBarTree( ids[2], ids[3] );
        	}
        	// interact with diagrams
        	else if ( ids[1] == ID_DIAGRAM )
        	{
			if ( mouseClick == MSE_CLICK_SINGLE &&
				mouseSide   == MSE_SIDE_LFT &&
				mouseDrag   == MSE_DRAG_FALSE )
			{
				dragIdxDgrm = ids[2];
				currIdxDgrm = ids[2];
				updateMarkBundles();

				if ( ids.size() == 4 )
				{
					if ( ids[3] == ID_DIAGRAM_CLSE )
						handleShowDiagram( ids[2] );
					else if ( ids[3] == ID_DIAGRAM_MORE )
					{
						// show menu
						if ( mediator->getView() == Mediator::VIEW_SIM )
							mediator->handleSendDgrm( this, true, false, false, true, true );
						else if ( mediator->getView() == Mediator::VIEW_TRACE )
							mediator->handleSendDgrm( this, false, true, true, true, true );

						showMenu = true;

						// no mouseup event is generated reset manually
						dragIdxDgrm = -1;
						mouseButton = MSE_BUTTON_UP;
						mouseSide   = MSE_SIDE_LFT;
						mouseClick  = MSE_CLICK_SINGLE;
						mouseDrag   = MSE_DRAG_FALSE;
					}
					else if ( ids[3] == ID_DIAGRAM_RWND )
						handleRwndDiagram( ids[2] );
					else if ( ids[3] == ID_DIAGRAM_PREV )
						handlePrevDiagram( ids[2] );
					else if ( ids[3] == ID_DIAGRAM_PLAY )
						handlePlayDiagram( ids[2] );
					else if ( ids[3] == ID_DIAGRAM_NEXT )
						handleNextDiagram( ids[2] );
				}
			}
			else if ( 	mouseSide   == MSE_SIDE_RGT &&
					mouseButton == MSE_BUTTON_DOWN /*&&
					mouseDrag   == MSE_DRAG_FALSE*/ )
			{
				// show menu
				if ( mediator->getView() == Mediator::VIEW_SIM )
				mediator->handleSendDgrm( this, true, false, false, true, true );
				else if ( mediator->getView() == Mediator::VIEW_TRACE )
				mediator->handleSendDgrm( this, false, true, true, true, true );

				showMenu = true;

				// no mouseup event is generated reset manually
				dragIdxDgrm = -1;
				mouseButton = MSE_BUTTON_UP;
				mouseSide   = MSE_SIDE_RGT;
				mouseClick  = MSE_CLICK_SINGLE;
				mouseDrag   = MSE_DRAG_FALSE;
			}
			else
			{
				canvas->clearToolTip();
				currIdxDgrm = ids[2];
				updateMarkBundles();

				ColorRGB col;
				VisUtils::mapColorCoolBlue( col );
				mediator->handleShowFrame(
				framesDgrm[currIdxDgrm][frameIdxDgrm[currIdxDgrm]],
				attrsDgrm[currIdxDgrm],
				col );
			}
        	}
    	}
    }
}


// ---------------------------------
void ArcDiagram::handleHoverCluster(
    const size_t &i,
    const size_t &j )
// ---------------------------------
{
    if ( ( i < mapPosToClust.size()    ) &&
         ( j < mapPosToClust[i].size() ) )
    {
        string msg;

        if ( i == 0 )
            msg = "All states";
        else
        {
            Cluster* clust = mapPosToClust[i][j];

            if (Value* value = clust->getAttribute()->getCurValue( clust->getAttrValIdx() )) {
              msg = value->getValue();
            }

            /* -*-
            Value* val;
            val = clust->getAttribute()->mapToValue( clust->getAttrValIdx() );
            if ( val != NULL )
                msg = val->getValue();
            else
                msg = "";
            val = NULL;
            */
        }
        canvas->showToolTip( msg );
    }
}


// -----------------------------------------------------
void ArcDiagram::handleHoverBundle( const int &bndlIdx )
// -----------------------------------------------------
{
    if ( 0 <= bndlIdx && bndlIdx < graph->getSizeBundles()  )
    {
        string  sepr  = "; ";
        string  lbls = "";
        Bundle* bndl = graph->getBundle( bndlIdx );
        bndl->getLabels( sepr, lbls );
        canvas->showToolTip( lbls );
        bndl = NULL;
    }
}


// ---------------------------------
void ArcDiagram::handleHoverBarTree(
    const int &i,
    const int &j )
// ---------------------------------
{
    if ( ( 0 <= i && static_cast <size_t> (i) < mapPosToClust.size()    ) &&
         ( 0 <= j && static_cast <size_t> (j) < mapPosToClust[i].size() ) )
    {
        string   msg;
        Cluster* clust;

        clust = mapPosToClust[i][j];
        msg = Utils::intToStr( clust->getSizeDescNodes() );
        canvas->showToolTip( msg );

        clust = NULL;
    }
}


// -----------------------------------------------------
void ArcDiagram::handleShowDiagram( const int &dgrmIdx )
// -----------------------------------------------------
{
	// diagram doesn't exist, add it
	if ( showDgrm[dgrmIdx] != true )
	{
        	showDiagram( dgrmIdx );
        	updateMarkBundles();
	}
	else
	// diagram exists, remove it
	{
        	hideDiagram( dgrmIdx );
        	currIdxDgrm = -1;
        	updateMarkBundles();
        	mediator->handleUnshowFrame();
	}
}


// ---------------------------------
void ArcDiagram::handleDragDiagram()
// ---------------------------------
{
    if ( 0 <= dragIdxDgrm && static_cast <size_t> (dragIdxDgrm) < posDgrm.size() )
    {
        double x1, y1;
        double x2, y2;

        canvas->getWorldCoords( xMousePrev, yMousePrev, x1, y1 );
        canvas->getWorldCoords( xMouseCur,  yMouseCur,  x2, y2 );

        posDgrm[dragIdxDgrm].x += (x2-x1);
        posDgrm[dragIdxDgrm].y += (y2-y1);
    }
}

// ---------------------------------
void ArcDiagram::handleDragDiagram(const int &dgrmIdx)
// ---------------------------------
{
    double x1, y1;
    double x2, y2;

    canvas->getWorldCoords( xMousePrev, yMousePrev, x1, y1 );
    canvas->getWorldCoords( xMouseCur,  yMouseCur,  x2, y2 );

    posDgrm[dgrmIdx].x += (x2-x1);
    posDgrm[dgrmIdx].y += (y2-y1);
}


// -----------------------------------------------------
void ArcDiagram::handleRwndDiagram( const size_t &dgrmIdx )
// -----------------------------------------------------
{
    if ( timerAnim->IsRunning() )
        timerAnim->Stop();

    if ( dgrmIdx != animIdxDgrm )
        animIdxDgrm = dgrmIdx;
    frameIdxDgrm[dgrmIdx] = 0;

    ColorRGB col;
    VisUtils::mapColorCoolBlue( col );
    mediator->handleShowFrame(
        framesDgrm[currIdxDgrm][frameIdxDgrm[currIdxDgrm]],
        attrsDgrm[currIdxDgrm],
        col );

    updateMarkBundles();
}


// -----------------------------------------------------
void ArcDiagram::handlePrevDiagram( const size_t &dgrmIdx )
// -----------------------------------------------------
{
    if ( timerAnim->IsRunning() )
        timerAnim->Stop();

    if ( dgrmIdx != animIdxDgrm )
        animIdxDgrm = dgrmIdx;

    frameIdxDgrm[dgrmIdx] -= 1;
    if ( frameIdxDgrm[dgrmIdx] < 0 )
        frameIdxDgrm[dgrmIdx] = framesDgrm[dgrmIdx].size()-1;

    ColorRGB col;
    VisUtils::mapColorCoolBlue( col );
    mediator->handleShowFrame(
        framesDgrm[currIdxDgrm][frameIdxDgrm[currIdxDgrm]],
        attrsDgrm[currIdxDgrm],
        col );

    updateMarkBundles();
}


// -----------------------------------------------------
void ArcDiagram::handlePlayDiagram( const size_t &dgrmIdx )
// -----------------------------------------------------
{
    if ( dgrmIdx == animIdxDgrm )
    {
        if ( timerAnim->IsRunning() )
        {
            timerAnim->Stop();

            ColorRGB col;
            VisUtils::mapColorCoolBlue( col );
            mediator->handleShowFrame(
                framesDgrm[currIdxDgrm][frameIdxDgrm[currIdxDgrm]],
                attrsDgrm[currIdxDgrm],
                col );
        }
        else
            timerAnim->Start( itvAnim );
    }
    else
    {
        animIdxDgrm = dgrmIdx;
        timerAnim->Start( itvAnim );
    }
}


// -----------------------------------------------------
void ArcDiagram::handleNextDiagram( const size_t &dgrmIdx )
// -----------------------------------------------------
{
    if ( timerAnim->IsRunning() )
        timerAnim->Stop();

    if ( dgrmIdx != animIdxDgrm )
        animIdxDgrm = dgrmIdx;

    frameIdxDgrm[dgrmIdx] += 1;
    if ( static_cast <size_t> (frameIdxDgrm[dgrmIdx]) >= framesDgrm[dgrmIdx].size() )
        frameIdxDgrm[dgrmIdx] = 0;

    ColorRGB col;
    VisUtils::mapColorCoolBlue( col );
    mediator->handleShowFrame(
        framesDgrm[currIdxDgrm][frameIdxDgrm[currIdxDgrm]],
        attrsDgrm[currIdxDgrm],
        col );

    updateMarkBundles();
}


// -----------------------------------------------
void ArcDiagram::showDiagram( const int &dgrmIdx )
// -----------------------------------------------
{
    Cluster* clust = graph->getLeaf( dgrmIdx );

    if ( clust != NULL )
    {
        Attribute*        attr;
        set< Attribute* > attrs;

        // show diagram
        showDgrm[dgrmIdx] = true;

        // find attributes linked to DOF's in diagram
        for ( int i = 0; i < diagram->getSizeShapes(); ++i )
        {
            // get result
            attr   = diagram->getShape(i)->getDOFXCtr()->getAttribute();
            if ( attr != NULL )
                attrs.insert( attr );

            attr = diagram->getShape(i)->getDOFYCtr()->getAttribute();
            if ( attr != NULL )
                attrs.insert( attr );

            attr = diagram->getShape(i)->getDOFWth()->getAttribute();
            if ( attr != NULL )
                attrs.insert( attr );

            attr = diagram->getShape(i)->getDOFHgt()->getAttribute();
            if ( attr != NULL )
                attrs.insert( attr );

            attr = diagram->getShape(i)->getDOFAgl()->getAttribute();
            if ( attr != NULL )
                attrs.insert( attr );

            attr = diagram->getShape(i)->getDOFCol()->getAttribute();
            if ( attr != NULL )
                attrs.insert( attr );

            attr = diagram->getShape(i)->getDOFOpa()->getAttribute();
            if ( attr != NULL )
                attrs.insert( attr );

	    attr = diagram->getShape(i)->getDOFText()->getAttribute();
	    if( attr != NULL )
		attrs.insert( attr );
        }


        // find attributes corresponding to path to root in clustering tree
        while ( clust != graph->getRoot() )
        {
            attrs.insert( clust->getAttribute() );
            clust = clust->getParent();
        }

        // update attrsDiagram
        attrsDgrm[dgrmIdx].clear();
        set< Attribute* >::iterator it;
        for ( it = attrs.begin(); it != attrs.end(); ++it )
            attrsDgrm[dgrmIdx].push_back( *it );

        // clear framesDgrm
        {
        for ( size_t i = 0; i < framesDgrm[dgrmIdx].size(); ++i )
            delete framesDgrm[dgrmIdx][i];
        }
        framesDgrm[dgrmIdx].clear();

        // update framesDgrm
        clust = graph->getLeaf( dgrmIdx );
        graph->calcAttrCombn(
            clust,
            attrsDgrm[dgrmIdx],
            framesDgrm[dgrmIdx] );

        // init frameIdxDgrm
        frameIdxDgrm[dgrmIdx] = 0;

        // update positions
        posDgrm[dgrmIdx].x = posLeaves[dgrmIdx].x + radLeaves;
        posDgrm[dgrmIdx].y = posLeaves[dgrmIdx].y - 0.2 - 2.0*radLeaves;

        // clear memory
        attrs.clear();
        attr = NULL;
    }

    clust = NULL;
}


// -----------------------------------------------
void ArcDiagram::hideDiagram( const size_t &dgrmIdx )
// -----------------------------------------------
{
    Cluster* clust = graph->getLeaf( dgrmIdx );

    if ( clust != NULL )
    {
        // hide diagram
        showDgrm[dgrmIdx] = false;

        // clear attributes linked to DOF's in diagram
        attrsDgrm[dgrmIdx].clear();

        // clear animation info
        if ( animIdxDgrm == dgrmIdx )
        {
            if ( timerAnim->IsRunning() )
                timerAnim->Stop();
            animIdxDgrm      = -1;
        }

        // clear position
        posDgrm[dgrmIdx].x = 0;
        posDgrm[dgrmIdx].y = 0;
    }

    clust = NULL;
}


// -- hit detection -------------------------------------------------


// --------------------------
void ArcDiagram::processHits(
    GLint hits,
    GLuint buffer[] )
// --------------------------
{
    GLuint *ptr;
    vector< int > ids;

    ptr = (GLuint*) buffer;

    if ( hits > 0 )
    {
        // if necassary, advance to closest hit
        if ( hits > 1 )
        {
            for ( int i = 0; i < ( hits-1 ); ++i )
            {
                int number = *ptr;
                ++ptr; // number;
                ++ptr; // z1
                ++ptr; // z2
                for ( int j = 0; j < number; ++j )
                    ++ptr; // names
            }
        }

        // last hit
        int number = *ptr;
        ++ptr; // number
        ++ptr; // z1
        ++ptr; // z2

        for ( int i = 0; i < number; ++i )
        {
            ids.push_back( *ptr );
            ++ptr;
        }

        handleHits( ids );
    }

    ptr = NULL;
}


// -- implement event table -----------------------------------------


BEGIN_EVENT_TABLE( ArcDiagram, wxEvtHandler )
    // menu bar
    EVT_TIMER( ID_TIMER, ArcDiagram::onTimer )
END_EVENT_TABLE()


// -- end -----------------------------------------------------------
