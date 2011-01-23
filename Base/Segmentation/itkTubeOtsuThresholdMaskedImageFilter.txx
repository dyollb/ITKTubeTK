/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkTubeOtsuThresholdMaskedImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright ( c ) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTubeOtsuThresholdMaskedImageFilter_txx
#define __itkTubeOtsuThresholdMaskedImageFilter_txx

#include "itkTubeOtsuThresholdMaskedImageFilter.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkTubeOtsuThresholdMaskedImageCalculator.h"
#include "itkProgressAccumulator.h"

namespace itk {

namespace tube {

template<class TInputImage, class TOutputImage>
OtsuThresholdMaskedImageFilter<TInputImage, TOutputImage>
::OtsuThresholdMaskedImageFilter()
{
  m_MaskImage      = NULL;
  m_OutsideValue   = NumericTraits<OutputPixelType>::Zero;
  m_InsideValue    = 1;
  m_Threshold      = NumericTraits<InputPixelType>::Zero;
  m_NumberOfHistogramBins = 128;
}

template<class TInputImage, class TOutputImage>
void
OtsuThresholdMaskedImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  typename ProgressAccumulator::Pointer progress =
    ProgressAccumulator::New();
  progress->SetMiniPipelineFilter( this );

  // Compute the Otsu Threshold for the input image
  typename OtsuThresholdMaskedImageCalculator<TInputImage>::Pointer otsu =
    OtsuThresholdMaskedImageCalculator<TInputImage>::New();
  otsu->SetImage ( this->GetInput() );
  if( m_MaskImage.IsNotNull() )
    {
    std::cout << "Setting mask." << std::endl;
    otsu->SetMaskImage ( m_MaskImage );
    }
  otsu->SetNumberOfHistogramBins ( m_NumberOfHistogramBins );
  otsu->Compute();
  m_Threshold = otsu->GetThreshold();

  typename BinaryThresholdImageFilter<TInputImage,TOutputImage>::Pointer
    threshold = BinaryThresholdImageFilter<TInputImage,TOutputImage>::New();

  progress->RegisterInternalFilter( threshold,.5f );
  threshold->GraftOutput ( this->GetOutput() );
  threshold->SetInput ( this->GetInput() );
  threshold->SetLowerThreshold(
    NumericTraits<InputPixelType>::NonpositiveMin() );
  threshold->SetUpperThreshold( otsu->GetThreshold() );
  threshold->SetInsideValue ( m_InsideValue );
  threshold->SetOutsideValue ( m_OutsideValue );
  threshold->Update();

  this->GraftOutput( threshold->GetOutput() );
}

template<class TInputImage, class TOutputImage>
void
OtsuThresholdMaskedImageFilter<TInputImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  TInputImage * input = const_cast<TInputImage *>( this->GetInput() );
  if( input )
    {
    input->SetRequestedRegionToLargestPossibleRegion();
    }
}

template<class TInputImage, class TOutputImage>
void
OtsuThresholdMaskedImageFilter<TInputImage,TOutputImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os,indent );

  if( m_MaskImage.IsNotNull() )
    {
    os << indent << "MaskImage: " << m_MaskImage << std::endl;
    }
  else
    {
    os << indent << "MaskImage: NULL" << std::endl;
    }
  os << indent << "OutsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(
       m_OutsideValue ) << std::endl;
  os << indent << "InsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(
       m_InsideValue ) << std::endl;
  os << indent << "NumberOfHistogramBins: "
     << m_NumberOfHistogramBins << std::endl;
  os << indent << "Threshold ( computed ): "
     << static_cast<typename NumericTraits<InputPixelType>::PrintType>(
       m_Threshold ) << std::endl;

}

}// end namespace tube

}// end namespace itk

#endif
