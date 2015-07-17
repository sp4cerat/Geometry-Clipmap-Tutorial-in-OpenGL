#pragma once
/*------------------------------------------------------*/
#define _USE_MATH_DEFINES
#include <windows.h>
#include <mmsystem.h>
#include <malloc.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <vector>
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
/*------------------------------------------------------*/
#define ushort unsigned short
#define uint unsigned int
#define uchar unsigned char
/*------------------------------------------------------*/
#define loop0i(end_l) for ( int i=0;i<end_l;++i )
#define loop0j(end_l) for ( int j=0;j<end_l;++j )
#define loopi(start_l,end_l) for ( int i=start_l;i<end_l;++i )
#define loopj(start_l,end_l) for ( int j=start_l;j<end_l;++j )
#define loopk(start_l,end_l) for ( int k=start_l;k<end_l;++k )
#define loopl(start_l,end_l) for ( int l=start_l;l<end_l;++l )
#define loopm(start_l,end_l) for ( int m=start_l;m<end_l;++m )
#define loopn(start_l,end_l) for ( int n=start_l;n<end_l;++n )
#define loop(a_l,start_l,end_l) for ( int a_l = start_l;a_l<end_l;++a_l )
/*------------------------------------------------------*/
#define loopij(_sti,_stj,_eni,_enj) loopi(_sti,_eni)loopj (_stj,_enj)
#define loopijk(_sti,_stj,_stk,_eni,_enj,_enk) loopi(_sti,_eni) loopj (_stj,_enj) loopk (_stk,_enk)
#define looplmn(_stl,_stm,_stn,_enl,_enm,_enn) loopl(_stl,_enl) loopm (_stm,_enm) loopn (_stn,_enn)
/*------------------------------------------------------*/
#define foreach(var, container) for( auto var = (container).begin(); var != (container).end(); ++var)
/*------------------------------------------------------*/
#define f_min min
#define f_max max
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define clamp(a_,b_,c_) min(max(a_,b_),c_)
//#define lerp(t, a, b) ( a + t * (b - a) )
//#define lerp(a_,b_,t_) ( a*(1-t_) + b*t_ )
/*------------------------------------------------------*/
#define error_stop(e){MessageBoxA(0,e,"some error",0);exit(0);}
/*------------------------------------------------------*/
