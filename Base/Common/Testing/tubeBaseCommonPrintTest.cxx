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

#include "itktubeDifferenceImageFilter2.h"

#include <itkImage.h>

#include <cstdlib>
#include <iostream>

int tubeBaseCommonPrintTest( int, char *[] )
{
  typedef itk::Image< float, 2 > ImageType;

  itk::tube::DifferenceImageFilter2< ImageType, ImageType >::Pointer
    differenceImageFilter2
    = itk::tube::DifferenceImageFilter2< ImageType, ImageType >::New();

  std::cout << "-------------itktubeDifferenceImageFilter2"
            << differenceImageFilter2 << std::endl;

  return EXIT_SUCCESS;
}
