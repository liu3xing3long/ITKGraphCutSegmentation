#ifndef __StandardGraphCutImageFilter_hxx_
#define __StandardGraphCutImageFilter_hxx_

namespace itk {
  template<typename TInputImage, typename TGreyImage, typename TOutputImage, typename TWeight>
  StandardGraphCutImageFilter<TInputImage, TGreyImage, TOutputImage, TWeight>
  ::StandardGraphCutImageFilter()
    : Superclass()
    , m_Sigma(0.1)
    , m_WeightScale( (itk::NumericTraits<double>::max() - 1) / 6.0 - itk::NumericTraits<double>::epsilon() )
  {
    this->SetNumberOfRequiredInputs(2);
  }

  template<typename TInputImage, typename TGreyImage, typename TOutputImage, typename TWeight>
  StandardGraphCutImageFilter<TInputImage, TGreyImage, TOutputImage, TWeight>
  ::~StandardGraphCutImageFilter()
  {}

  template<typename TInputImage, typename TGreyImage, typename TOutputImage, typename TWeight>
  TWeight StandardGraphCutImageFilter<TInputImage, TGreyImage, TOutputImage, TWeight>
  ::ComputeRegionalTerm(InputIndexType index, LabelType label)  {
    // Grab input image
    InputImageConstPointerType labelImage = Superclass::GetInput();

    if (labelImage->GetPixel(index) == Superclass::GetForegroundLabel()) {
      return (label == Superclass::GetForegroundLabel()) ? itk::NumericTraits<TWeight>::max() : (TWeight)0;
    } else if (labelImage->GetPixel(index) == Superclass::GetBackgroundLabel()) {
      return (label == Superclass::GetBackgroundLabel()) ? itk::NumericTraits<TWeight>::max() : (TWeight)0;
    } else {
      // We could add a lambda R(A) here.
      return (TWeight)0;
    }
  }

  template<typename TInputImage, typename TGreyImage, typename TOutputImage, typename TWeight>
  TWeight StandardGraphCutImageFilter<TInputImage, TGreyImage, TOutputImage, TWeight>
  ::ComputeBoundaryTerm(InputIndexType firstIndex, InputIndexType secondIndex) {
    // Get the greyscale image and the pixel values
    GreyImageConstPointerType greyImage = GetGreyInput();
    GreyImagePixelType firstPixelValue = greyImage->GetPixel(firstIndex);
    GreyImagePixelType secondPixelValue = greyImage->GetPixel(secondIndex);

    // Notice the constraint that k = 1 + max sum B(p,q)
    // I rearranged this such that we can scale the boundary term with TWeight
    //  B(p,q) \in [0,1]
    //  weight = (type::max() - 1) / 6 - type::epsilon
    // We're dealing with six neighborhood topologies so k = 1 + 6 * B(p,q)
    double difference = firstPixelValue - secondPixelValue;
    double weight = vcl_exp( - vcl_pow(difference, 2) / (2 * vcl_pow(m_Sigma, 2)) );
    return static_cast<TWeight>(m_WeightScale*weight);
  }

  template<typename TInputImage, typename TGreyImage, typename TOutputImage, typename TWeight>
  void StandardGraphCutImageFilter<TInputImage, TGreyImage, TOutputImage, TWeight>
  ::PrintSelf(std::ostream & os, Indent indent) const {
      Superclass::PrintSelf(os, indent);
      os << indent << "Sigma:           " << m_Sigma << std::endl;
  }

} /* namespace itk */

#endif /* __StandardGraphCutImageFilter_hxx_ */