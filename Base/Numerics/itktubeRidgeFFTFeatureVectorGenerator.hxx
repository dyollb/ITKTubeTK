/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/

#ifndef __itktubeRidgeFFTFeatureVectorGenerator_hxx
#define __itktubeRidgeFFTFeatureVectorGenerator_hxx

#include "itktubeRidgeFFTFeatureVectorGenerator.h"
#include "itktubeRidgeFFTFilter.h"
#include "tubeMatrixMath.h"

#include <itkImage.h>
#include <itkProgressReporter.h>
#include <itkTimeProbesCollectorBase.h>

#include <limits>

namespace itk
{

namespace tube
{

template< class TImage >
RidgeFFTFeatureVectorGenerator< TImage >
::RidgeFFTFeatureVectorGenerator( void )
{
  m_IntensityMin = 0;
  m_IntensityMax = 0;
  m_Scales.resize( 0 );
  m_FeatureImageList.resize( 0 );
}

template< class TImage >
RidgeFFTFeatureVectorGenerator< TImage >
::~RidgeFFTFeatureVectorGenerator( void )
{
}

template< class TImage >
void
RidgeFFTFeatureVectorGenerator< TImage >
::SetIntensityMin( float intensityMin )
{
  m_IntensityMin = intensityMin;
}

template< class TImage >
float
RidgeFFTFeatureVectorGenerator< TImage >
::GetIntensityMin( void ) const
{
  return m_IntensityMin;
}

template< class TImage >
void
RidgeFFTFeatureVectorGenerator< TImage >
::SetIntensityMax( float intensityMax )
{
  m_IntensityMax = intensityMax;
}

template< class TImage >
float
RidgeFFTFeatureVectorGenerator< TImage >
::GetIntensityMax( void ) const
{
  return m_IntensityMax;
}

template< class TImage >
unsigned int
RidgeFFTFeatureVectorGenerator< TImage >
::GetNumberOfFeatures( void ) const
{
  const unsigned int numFeatures = m_Scales.size() * 5 + 6;

  return numFeatures;
}


template< class TImage >
void
RidgeFFTFeatureVectorGenerator< TImage >
::GenerateData( void )
{
  typedef RidgeFFTFilter< TImage > RidgeFilterType;
  typename RidgeFilterType::Pointer ridgeF = RidgeFilterType::New();
  ridgeF->SetInput( this->m_InputImageList[0] );

  const unsigned int numFeatures = this->GetNumberOfFeatures();

  m_FeatureImageList.resize( numFeatures );

  unsigned int feat = 0;
  for( unsigned int s=0; s<m_Scales.size(); s++ )
    {
    ridgeF->SetScale( m_Scales[s] );
    ridgeF->Update();

    m_FeatureImageList[feat++] = ridgeF->GetIntensity();
    m_FeatureImageList[feat++] = ridgeF->GetRidgeness();
    m_FeatureImageList[feat++] = ridgeF->GetRoundness();
    m_FeatureImageList[feat++] = ridgeF->GetCurvature();
    m_FeatureImageList[feat++] = ridgeF->GetLevelness();
    }

  typename FeatureImageType::RegionType region = 
    this->m_InputImageList[0]->GetLargestPossibleRegion();
  for( unsigned int f=numFeatures-6; f<numFeatures; ++f )
    {
    m_FeatureImageList[f] = FeatureImageType::New();
    m_FeatureImageList[f]->CopyInformation( this->m_InputImageList[0] );
    m_FeatureImageList[f]->SetRegions( region );
    m_FeatureImageList[f]->Allocate();
    }

  typedef ImageRegionIterator< FeatureImageType >  IterType;
  std::vector< IterType > iterF( numFeatures );
  for( unsigned int f=0; f<numFeatures; ++f )
    {
    iterF[f] = IterType( m_FeatureImageList[f], region );
    }
  unsigned int foScale = numFeatures - 6;
  unsigned int foFeat = numFeatures - 5;
  while( !iterF[0].IsAtEnd() )
    {
    unsigned int extremeS = 0;
    for( unsigned int s=0; s<m_Scales.size(); s++ )
      {
      feat = s * 5;
      for( unsigned int f=0; f<5; ++f )
        {
        if( s == 0 || iterF[ feat + f ].Get() > iterF[ foFeat + f ].Get() )
          {
          iterF[ foFeat + f ].Set( iterF[ feat + f ].Get() );
          if( f == 0 )
            {
            extremeS = s;
            }
          }
        }
      iterF[ foScale ].Set( m_Scales[ extremeS ] );
      }
    for( unsigned int f=0; f<numFeatures; ++f )
      {
      ++iterF[ f ];
      }
    }
}

template< class TImage >
typename RidgeFFTFeatureVectorGenerator< TImage >::FeatureVectorType
RidgeFFTFeatureVectorGenerator< TImage >
::GetFeatureVector( const IndexType & indx ) const
{
  const unsigned int numFeatures = this->GetNumberOfFeatures();

  FeatureVectorType featureVector( numFeatures );

  for( unsigned int f=0; f<numFeatures; ++f )
    {
    featureVector[f] = m_FeatureImageList[f]->GetPixel( indx );
    }

  return featureVector;
}

template< class TImage >
typename RidgeFFTFeatureVectorGenerator< TImage >::FeatureValueType
RidgeFFTFeatureVectorGenerator< TImage >
::GetFeatureVectorValue( const IndexType & indx, unsigned int fNum ) const
{
  return this->m_FeatureImageList[ fNum ]->GetPixel( indx );
}

template< class TImage >
typename RidgeFFTFeatureVectorGenerator< TImage >::FeatureImageType::Pointer
RidgeFFTFeatureVectorGenerator< TImage >
::GetFeatureImage( unsigned int i ) const
{
  return this->m_FeatureImageList[ i ];
}

template< class TImage >
void
RidgeFFTFeatureVectorGenerator< TImage >
::SetScales( const RidgeScalesType & scales )
{
  this->m_Scales = scales;
}

template< class TImage >
const std::vector< double > &
RidgeFFTFeatureVectorGenerator< TImage >
::GetScales( void ) const
{
  return this->m_Scales;
}

template< class TImage >
void
RidgeFFTFeatureVectorGenerator< TImage >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Scales.size() = " << m_Scales.size() << std::endl;
}

} // End namespace tube

} // End namespace itk

#endif // End !defined(__itktubeRidgeFFTFeatureVectorGenerator_hxx)