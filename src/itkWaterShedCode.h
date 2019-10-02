#include <iostream>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <dos.h>

using namespace std;

#include "itkVectorGradientAnisotropicDiffusionImageFilter.h"
#include "itkVectorGradientMagnitudeImageFilter.h"
#include "itkWatershedImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVectorCastImageFilter.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkGDCMImageIO.h"
#include "itkMetaImageIO.h"

#include "itkCommand.h"
#include "itkImage.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "vtkImageData.h"

#include "vtkDICOMImageReader.h"
#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"


template <typename ITK_Exporter, typename VTK_Importer> void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
    importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
    importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
    importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
    importer->SetSpacingCallback(exporter->GetSpacingCallback());
    importer->SetOriginCallback(exporter->GetOriginCallback());
    importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
    importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
    importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
    importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
    importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
    importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
    importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

template <typename VTK_Exporter, typename ITK_Importer> void ConnectPipelines(VTK_Exporter* exporter, ITK_Importer importer)
{
    importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
    importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
    importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
    importer->SetSpacingCallback(exporter->GetSpacingCallback());
    importer->SetOriginCallback(exporter->GetOriginCallback());
    importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
    importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
    importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
    importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
    importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
    importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
    importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

int watershedSegmentation(char *file_name)
{
    /*if (argc < 8 )
    {
        std::cerr << "Missing Parameters " << std::endl;
        std::cerr << "Usage: " << argv[0];
        std::cerr << " inputImage imageType conductanceTerm diffusionIterations lowerThreshold outputScaleLevel gradientMode " << std::endl;
        return EXIT_FAILURE;
    }*/

    try
    {

        char imageType[1000], conductanceTerm[1000], diffusionIterations[1000], lowerThreshold[1000], outputScaleLevel[1000], gradientMode[1000];

        cout<< "Type image type- '-IMAGE'/'-DICOM':";
        cin>>imageType;

        cout<< "Enter conductance term value: ";
        cin>>conductanceTerm;

        cout<< "Enter diffusion iterations value: ";
        cin>>diffusionIterations;

        cout<< "Enter lower threshold value: ";
        cin>>lowerThreshold;

        cout<<"Enter output scale level value: ";
        cin>>outputScaleLevel;

        cout<<"Enter gradient mode value: ";
        cin>> gradientMode;

        typedef itk::RGBPixel< unsigned char >       RGBPixelType;

        typedef itk::Image< RGBPixelType, 2 >        RGBImageType;

        typedef itk::Vector< float, 3 >              VectorPixelType;
        typedef itk::Image< VectorPixelType, 2 >     VectorImageType;
        typedef itk::Image< itk::IdentifierType, 2 > LabeledImageType;
        typedef itk::Image< float, 2 >               ScalarImageType;

        typedef itk::ImageFileReader< RGBImageType >   FileReaderType;

        typedef itk::VectorCastImageFilter< RGBImageType, VectorImageType > CastFilterType;
        typedef itk::VectorGradientAnisotropicDiffusionImageFilter<VectorImageType, VectorImageType > DiffusionFilterType;
        typedef itk::VectorGradientMagnitudeImageFilter< VectorImageType > GradientMagnitudeFilterType;
        typedef itk::WatershedImageFilter< ScalarImageType > WatershedFilterType;

        typedef itk::GDCMImageIO           ImageIOType;
        ImageIOType::Pointer gdcmImageIO = ImageIOType::New();

        FileReaderType::Pointer reader = FileReaderType::New();

        reader->SetFileName(file_name);

        for(int i = 0; imageType[i]!='\0'; i++)
            imageType[i] = toupper(imageType[i]);

        if(!strcmp(imageType, "-DICOM"))
        {
            reader->SetImageIO(gdcmImageIO);
        }
        try
        {
            reader->Update();
        }
        catch (itk::ExceptionObject & e)
        {
            std::cerr << "exception in file reader " << std::endl;
            std::cerr << e << std::endl;
            return EXIT_FAILURE;
        }

        CastFilterType::Pointer caster = CastFilterType::New();

        DiffusionFilterType::Pointer diffusion = DiffusionFilterType::New();
        diffusion->SetNumberOfIterations( atoi(diffusionIterations) );
        diffusion->SetConductanceParameter( atof(conductanceTerm) );
        diffusion->SetTimeStep(0.125);

        GradientMagnitudeFilterType::Pointer gradient = GradientMagnitudeFilterType::New();
        gradient->SetUsePrincipleComponents(atoi(gradientMode));

        WatershedFilterType::Pointer watershed = WatershedFilterType::New();
        watershed->SetLevel( atof(outputScaleLevel) );
        watershed->SetThreshold( atof(lowerThreshold) );

        typedef itk::Functor::ScalarToRGBPixelFunctor<unsigned long> ColorMapFunctorType;
        typedef itk::UnaryFunctorImageFilter<LabeledImageType, RGBImageType, ColorMapFunctorType> ColorMapFilterType;
        ColorMapFilterType::Pointer colormapper = ColorMapFilterType::New();

        caster->SetInput(reader->GetOutput());
        diffusion->SetInput(caster->GetOutput());
        gradient->SetInput(diffusion->GetOutput());
        watershed->SetInput(gradient->GetOutput());
        colormapper->SetInput(watershed->GetOutput());

        typedef itk::VTKImageExport< RGBImageType > ExportFilterType; //
        ExportFilterType::Pointer itkExporter = ExportFilterType::New(); //

        itkExporter->SetInput( colormapper->GetOutput() ); //

        vtkImageImport* vtkImporter = vtkImageImport::New();  //
        ConnectPipelines(itkExporter, vtkImporter);

        typedef itk::VTKImageImport< RGBImageType > ImportFilterType; //
        ImportFilterType::Pointer itkImporter = ImportFilterType::New(); //


        vtkImageExport* vtkExporter = vtkImageExport::New();
        ConnectPipelines(vtkExporter, itkImporter);

#if VTK_MAJOR_VERSION <= 5
        vtkExporter->SetInput( vtkImporter->GetOutput() );
#else
        vtkImporter->Update();
        vtkExporter->SetInputData( vtkImporter->GetOutput() );
#endif

        vtkImageActor* actor = vtkImageActor::New();
#if VTK_MAJOR_VERSION <= 5
        actor->SetInput(vtkImporter->GetOutput());
#else
        actor->SetInputData(vtkImporter->GetOutput());
#endif

        vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();

        vtkRenderer* renderer = vtkRenderer::New();
        renderer->AddActor(actor);
        renderer->SetBackground(0.4392, 0.5020, 0.5647);

        vtkRenderWindow* renWin = vtkRenderWindow::New();
        renWin->SetSize(500, 500);
        renWin->AddRenderer(renderer);

        vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
        iren->SetRenderWindow(renWin);
        iren->SetInteractorStyle( interactorStyle );


        renWin->Render();
        iren->Start();

        actor->Delete();
        interactorStyle->Delete();
        vtkImporter->Delete();
        vtkExporter->Delete();
        renWin->Delete();
        renderer->Delete();
        iren->Delete();

    }
    catch (itk::ExceptionObject &e)
    {
        std::cerr << e << std::endl;
        return EXIT_FAILURE;
    }
}
