#include "registration.h"

registration::registration(QWidget *parent , Qt::WFlags flags)
    :QWidget(parent, flags)
{

}

void registration::itkDemonsRegistration(char* fixedImageName, char* movingImageName)
{
    const unsigned int Dimension = 2;
    typedef unsigned char PixelType;//unsigned short

    typedef itk::Image< PixelType, Dimension >  FixedImageType;
    typedef itk::Image< PixelType, Dimension >  MovingImageType;

    typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
    typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

    FixedImageReaderType::Pointer fixedImageReader   = FixedImageReaderType::New();
    MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

    fixedImageReader->SetFileName(fixedImageName);
    movingImageReader->SetFileName(movingImageName);

    typedef float                                      InternalPixelType;
    typedef itk::Image< InternalPixelType, Dimension > InternalImageType;
    typedef itk::CastImageFilter< FixedImageType,
        InternalImageType >  FixedImageCasterType;
    typedef itk::CastImageFilter< MovingImageType,
        InternalImageType >  MovingImageCasterType;

    FixedImageCasterType::Pointer fixedImageCaster = FixedImageCasterType::New();
    MovingImageCasterType::Pointer movingImageCaster
        = MovingImageCasterType::New();

    fixedImageCaster->SetInput( fixedImageReader->GetOutput() );
    movingImageCaster->SetInput( movingImageReader->GetOutput() );

    typedef itk::HistogramMatchingImageFilter<
        InternalImageType,
        InternalImageType >   MatchingFilterType;
    MatchingFilterType::Pointer matcher = MatchingFilterType::New();

    matcher->SetInput( movingImageCaster->GetOutput() );
    matcher->SetReferenceImage( fixedImageCaster->GetOutput() );

    matcher->SetNumberOfHistogramLevels( 1024 );
    matcher->SetNumberOfMatchPoints( 7 );

    matcher->ThresholdAtMeanIntensityOn();

    typedef itk::Vector< float, Dimension >           VectorPixelType;
    typedef itk::Image<  VectorPixelType, Dimension > DisplacementFieldType;
    typedef itk::DemonsRegistrationFilter<
        InternalImageType,
        InternalImageType,
        DisplacementFieldType> RegistrationFilterType;
    RegistrationFilterType::Pointer filter = RegistrationFilterType::New();

    CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    filter->AddObserver( itk::IterationEvent(), observer );

    filter->SetFixedImage( fixedImageCaster->GetOutput() );
    filter->SetMovingImage( matcher->GetOutput() );

    filter->SetNumberOfIterations( 50 );
    filter->SetStandardDeviations( 1.0 );

    filter->Update();

    typedef itk::WarpImageFilter<
        MovingImageType,
        MovingImageType,
        DisplacementFieldType  >     WarperType;
    typedef itk::LinearInterpolateImageFunction<
        MovingImageType,
        double          >  InterpolatorType;
    WarperType::Pointer warper = WarperType::New();
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

    warper->SetInput( movingImageReader->GetOutput() );
    warper->SetInterpolator( interpolator );
    warper->SetOutputSpacing( fixedImage->GetSpacing() );
    warper->SetOutputOrigin( fixedImage->GetOrigin() );
    warper->SetOutputDirection( fixedImage->GetDirection() );

    warper->SetDisplacementField( filter->GetOutput() );

    typedef  unsigned char                           OutputPixelType;
    typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
    typedef itk::CastImageFilter<
        MovingImageType,
        OutputImageType >          CastFilterType;
    typedef itk::ImageFileWriter< OutputImageType >  WriterType;

    WriterType::Pointer      writer =  WriterType::New();
    CastFilterType::Pointer  caster =  CastFilterType::New();
    outputFileName = ("../data/registration/registration.bmp");
    writer->SetFileName(outputFileName);

    caster->SetInput( warper->GetOutput() );
    writer->SetInput( caster->GetOutput()   );
    writer->Update();

    emit returnOutputFileName(outputFileName);//传递参数用指针
}

registration::~registration()
{

}