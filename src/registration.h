#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QtGui/QMainWindow>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkWarpImageFilter.h"

class registration : public QWidget
{
    Q_OBJECT

public:
    registration(QWidget *parent = 0, Qt::WFlags flags = 0);//���캯��
    ~registration();//��������

    //�Զ����Ա����
private:
    char* outputFileName;
    //�Զ����Ա����
public:
    void itkDemonsRegistration(char* fixedImageName, char* movingImageName);

    //�Զ����ź�&��
signals:
    void returnOutputFileName(char* fileName);
};

class CommandIterationUpdate : public itk::Command
{
public:
    typedef  CommandIterationUpdate                     Self;
    typedef  itk::Command                               Superclass;
    typedef  itk::SmartPointer<CommandIterationUpdate>  Pointer;
    itkNewMacro( CommandIterationUpdate );
protected:
    CommandIterationUpdate() {};

    typedef itk::Image< float, 2 >            InternalImageType;
    typedef itk::Vector< float, 2 >           VectorPixelType;
    typedef itk::Image<  VectorPixelType, 2 > DisplacementFieldType;

    typedef itk::DemonsRegistrationFilter<
        InternalImageType,
        InternalImageType,
        DisplacementFieldType>   RegistrationFilterType;

public:

    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
        Execute( (const itk::Object *)caller, event);
    }

    void Execute(const itk::Object * object, const itk::EventObject & event)
    {
        const RegistrationFilterType * filter = static_cast< const RegistrationFilterType * >( object );
        if( !(itk::IterationEvent().CheckEvent( &event )) )
        {
            return;
        }
        std::cout << filter->GetMetric() << std::endl;
    }
};

#endif // REGISTRATION_H
