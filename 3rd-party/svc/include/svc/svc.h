/*
   SVC -- the SVC (Systems Validation Centre) file format library

   Copyright (C) 2000  Stichting Mathematisch Centrum, Amsterdam,
                       The  Netherlands

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   $Id: svc.h,v 1.3 2008/09/30 08:22:51 bertl Exp $ */

/* Interface for the SVC level 2 interface */

#ifndef _SVC_H
#define _SVC_H

#include "aterm1.h"
#include "svc1.h"



  typedef SVCint SVCstateIndex;
  typedef SVCint SVClabelIndex;
  typedef SVCint SVCparameterIndex;

  typedef struct
  {
    ltsFile file;
    struct ltsHeader header;
    long transitionNumber;
  } SVCfile;




  /* Datatype 'SVClabel' */

  SVClabelIndex SVCnewLabel(SVCfile*, ATerm, bool*);
  SVClabelIndex SVCaterm2Label(SVCfile*, ATerm);
  ATerm SVClabel2ATerm(SVCfile*, SVClabelIndex);

  /* Datatype 'SVCstate' */

  SVCstateIndex SVCnewState(SVCfile*, ATerm, bool*);
  SVCstateIndex SVCaterm2State(SVCfile*, ATerm);
  ATerm SVCstate2ATerm(SVCfile*, SVCstateIndex);

  /* Datatype 'SVCparameter' */

  SVCparameterIndex SVCnewParameter(SVCfile*, ATerm, bool*);
  SVCparameterIndex SVCaterm2Parameter(SVCfile*, ATerm);
  ATerm SVCparameter2ATerm(SVCfile*, SVCparameterIndex);

  /* Datatype 'SVCTransition' */

  int SVCputTransition(SVCfile*, SVCstateIndex,
                       SVClabelIndex, SVCstateIndex, SVCparameterIndex);

  int SVCgetNextTransition(SVCfile*,
                           SVCstateIndex*, SVClabelIndex*,
                           SVCstateIndex*, SVCparameterIndex*);

  /* File interface */

  int SVCopen(SVCfile*, char*, SVCfileMode, bool*);
  int SVCclose(SVCfile*);

  /* Functions to put and get header information */

  char* SVCgetFormatVersion(SVCfile*);
  bool SVCgetIndexFlag(SVCfile*);
  SVCstateIndex SVCgetInitialState(SVCfile*);
  int SVCsetInitialState(SVCfile*, SVCstateIndex);
  char* SVCgetComments(SVCfile*);
  int SVCsetComments(SVCfile*, char*);
  char* SVCgetCreator(SVCfile*);
  int SVCsetCreator(SVCfile*, char*);
  char* SVCgetType(SVCfile*);
  int SVCsetType(SVCfile*, char*);
  char* SVCgetVersion(SVCfile*);
  int SVCsetVersion(SVCfile*, char*);
  char* SVCgetDate(SVCfile*);
  char* SVCgetFilename(SVCfile*);
  SVCint SVCnumStates(SVCfile*);
  SVCint SVCnumLabels(SVCfile*);
  SVCint SVCnumParameters(SVCfile*);
  SVCint SVCnumTransitions(SVCfile*);

  char* SVCerror(int);

#endif
