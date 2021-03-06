#include "itkConfidenceConnectedImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkMetaImageReader.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkGPUVolumeRayCastMapper.h>

#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkImageActor.h"

template <typename ITK_Exporter, typename VTK_Importer> void ConnectPipelines2(ITK_Exporter exporter, VTK_Importer* importer)
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

template <typename VTK_Exporter, typename ITK_Importer> void ConnectPipelines2(VTK_Exporter* exporter, ITK_Importer importer)
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


char buffer[1000];
char tmp[1000];

void getFileName(char *inputFile)
{
    int l = strlen(inputFile);
    int x = 0;
    for(int i = l-1; i>-1; i--)
    {
        if(inputFile[i]=='\\')
            break;
        buffer[x] = inputFile[i];
        x++;
    }
    strrev(buffer);
    //strcat(buffer,".mha");
    return;
}

void getOutputPath(char *inputFile)
{
    int l = strlen(inputFile);
    int x = 0;
    bool flag = false;
    for(int i = l-1; i>-1; i--)
    {
        if(inputFile[i]=='\\' && !flag)
            flag = true;
        if(flag)
        {
            tmp[x] = inputFile[i];
            x++;
        }
    }
    strrev(tmp);
    return;
}


void confidenceSeg()
{

    cout<< "Enter input image file path: ";
    char inputImagePath[1000];
    cin>>inputImagePath;

//    char outputImageName[1000];
//    getFileName(inputImagePath);
//    strcpy(outputImageName,"Segmented_");
//    strcat(outputImageName,buffer);

    typedef   float           InternalPixelType;
    const     unsigned int    Dimension = 3;
    typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;

    typedef unsigned char                            OutputPixelType;
    typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
    OutputImageType::Pointer image;

    typedef itk::CastImageFilter< InternalImageType, OutputImageType > CastingFilterType;
    CastingFilterType::Pointer caster = CastingFilterType::New();


    typedef  itk::ImageFileReader< InternalImageType > ReaderType;
    //typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

    ReaderType::Pointer reader = ReaderType::New();
    //WriterType::Pointer writer = WriterType::New();

    reader->SetFileName( inputImagePath );
    //writer->SetFileName( outputImageName );

    typedef itk::CurvatureFlowImageFilter< InternalImageType, InternalImageType > CurvatureFlowImageFilterType;
    CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();

    typedef itk::ConfidenceConnectedImageFilter<InternalImageType, InternalImageType> ConnectedFilterType;
    ConnectedFilterType::Pointer confidenceConnected = ConnectedFilterType::New();

    smoothing->SetInput( reader->GetOutput() );
    confidenceConnected->SetInput( smoothing->GetOutput() );
    caster->SetInput( confidenceConnected->GetOutput() );
    //writer->SetInput( caster->GetOutput() );

    smoothing->SetNumberOfIterations( 7 );
    smoothing->SetTimeStep( 0.05 );

    confidenceConnected->SetMultiplier( 2.5 );
    confidenceConnected->SetNumberOfIterations( 5 );
    confidenceConnected->SetInitialNeighborhoodRadius( 2 );
    confidenceConnected->SetReplaceValue( 255 );

    InternalImageType::IndexType index1;
    index1[0] = 118;
    index1[1] = 133;
    index1[2] = 92;
    confidenceConnected->AddSeed( index1 );

    InternalImageType::IndexType index2;
    index2[0] = 63;
    index2[1] = 135;
    index2[2] = 94;
    confidenceConnected->AddSeed( index2 );

    InternalImageType::IndexType index3;
    index3[0] = 63;
    index3[1] = 157;
    index3[2] = 90;
    confidenceConnected->AddSeed( index3 );

    InternalImageType::IndexType index4;
    index4[0] = 111;
    index4[1] = 150;
    index4[2] = 90;
    confidenceConnected->AddSeed( index4 );

    InternalImageType::IndexType index5;
    index5[0] = 111;
    index5[1] = 50;
    index5[2] = 88;
    confidenceConnected->AddSeed( index5 );

	// VTK Pipeline

    typedef itk::VTKImageExport< OutputImageType > ExportFilterType; //
	ExportFilterType::Pointer itkExporter = ExportFilterType::New(); //

	itkExporter->SetInput(caster->GetOutput()); //

	vtkImageImport* vtkImporter = vtkImageImport::New();  //
	ConnectPipelines2(itkExporter, vtkImporter);


	typedef itk::VTKImageImport< OutputImageType > ImportFilterType; //
	ImportFilterType::Pointer itkImporter = ImportFilterType::New(); //


	vtkImageExport* vtkExporter = vtkImageExport::New();
	ConnectPipelines2(vtkExporter, itkImporter);

#if VTK_MAJOR_VERSION <= 5
	vtkExporter->SetInput(vtkImporter->GetOutput());
#else
	vtkImporter->Update();
	vtkExporter->SetInputData(vtkImporter->GetOutput());
#endif

	//vtkImageActor* actor = vtkImageActor::New();
	vtkImageData	*imageData = vtkImageData::New();
#if VTK_MAJOR_VERSION <= 5
	//actor->SetInput(vtkImporter->GetOutput());
	imageData->ShallowCopy(vtkImporter->GetOutput());
#else
	//actor->SetInputData(vtkImporter->GetOutput());
	imageData->ShallowCopy(vtkImporter->GetOutput());
#endif

	vtkGPUVolumeRayCastMapper *mapperVolume = vtkGPUVolumeRayCastMapper::New();
	mapperVolume->SetBlendModeToComposite();
	mapperVolume->SetInputData(imageData);

	vtkPiecewiseFunction *gradientOpacity = vtkPiecewiseFunction::New();

	gradientOpacity->AddPoint(1, 0.0);
	gradientOpacity->AddPoint(5, 0.1);
	gradientOpacity->AddPoint(100, 1.0);

	vtkPiecewiseFunction *scalarOpacity = vtkPiecewiseFunction::New();

	scalarOpacity->AddPoint(0, 0.0);
	for (int i = 1; i < 150; i++)
	{
		scalarOpacity->AddPoint(i, 0.25);
	}

	vtkColorTransferFunction *color = vtkColorTransferFunction::New();

	color->AddRGBPoint(0, 0.0, 0.0, 0.0);
	color->AddRGBPoint(1, 0.9607843137254902, 0.9607843137254902, 0.9607843137254902);
	color->AddRGBPoint(2, 0.34509803921568627, 0.41568627450980394, 0.8431372549019608);
	color->AddRGBPoint(3, 0.34509803921568627, 0.41568627450980394, 0.8431372549019608);
	color->AddRGBPoint(4, 0.8666666666666667, 0.9725490196078431, 0.6431372549019608);
	color->AddRGBPoint(5, 0.9019607843137255, 0.5803921568627451, 0.13333333333333333);
	color->AddRGBPoint(6, 0.0, 0.4627450980392157, 0.054901960784313725);
	color->AddRGBPoint(7, 0.47843137254901963, 0.7294117647058823, 0.8627450980392157);
	color->AddRGBPoint(8, 0.9254901960784314, 0.050980392156862744, 0.6901960784313725);
	color->AddRGBPoint(9, 0.047058823529411764, 0.18823529411764706, 1.0);
	color->AddRGBPoint(10, 0.34509803921568627, 0.41568627450980394, 0.8431372549019608);
	color->AddRGBPoint(11, 0.8627450980392157, 0.8470588235294118, 0.0784313725490196);
	color->AddRGBPoint(12, 0.403921568627451, 1.0, 1.0);
	color->AddRGBPoint(13, 0.34509803921568627, 0.41568627450980394, 0.8431372549019608);
	color->AddRGBPoint(14, 1.0, 0.6470588235294118, 0.0);
	color->AddRGBPoint(15, 0.6470588235294118, 0.0, 1.0);
	color->AddRGBPoint(16, 0.6470588235294118, 0.16470588235294117, 0.16470588235294117);
	color->AddRGBPoint(17, 0.2, 0.19607843137254902, 0.5294117647058824);
	color->AddRGBPoint(18, 0.803921568627451, 0.24313725490196078, 0.3058823529411765);
	color->AddRGBPoint(19, 0.9607843137254902, 0.9607843137254902, 0.9607843137254902);
	color->AddRGBPoint(20, 0.34509803921568627, 0.41568627450980394, 0.8431372549019608);
	color->AddRGBPoint(21, 0.34509803921568627, 0.41568627450980394, 0.8431372549019608);
	color->AddRGBPoint(22, 0.8666666666666667, 0.9725490196078431, 0.6431372549019608);
	color->AddRGBPoint(23, 0.9019607843137255, 0.5803921568627451, 0.13333333333333333);
	color->AddRGBPoint(24, 0.0, 0.4627450980392157, 0.054901960784313725);
	color->AddRGBPoint(25, 0.47843137254901963, 0.7294117647058823, 0.8627450980392157);
	color->AddRGBPoint(26, 0.9254901960784314, 0.050980392156862744, 0.6901960784313725);
	color->AddRGBPoint(27, 0.050980392156862744, 0.18823529411764706, 1.0);
	color->AddRGBPoint(28, 0.8627450980392157, 0.8470588235294118, 0.0784313725490196);
	color->AddRGBPoint(29, 0.403921568627451, 1.0, 1.0);
	color->AddRGBPoint(30, 1.0, 0.6470588235294118, 0.0);
	color->AddRGBPoint(31, 0.6470588235294118, 0.16470588235294117, 0.16470588235294117);
	color->AddRGBPoint(32, 0.5294117647058824, 0.807843137254902, 0.9215686274509803);
	color->AddRGBPoint(33, 0.47843137254901963, 0.5294117647058824, 0.19607843137254902);
	color->AddRGBPoint(34, 0.47843137254901963, 0.5294117647058824, 0.19607843137254902);
	color->AddRGBPoint(35, 0.47058823529411764, 0.7450980392156863, 0.5882352941176471);
	color->AddRGBPoint(36, 1.0, 0.5803921568627451, 0.0392156862745098);
	color->AddRGBPoint(37, 1.0, 0.5803921568627451, 0.0392156862745098);
	color->AddRGBPoint(38, 0.050980392156862744, 0.18823529411764706, 1.0);
	color->AddRGBPoint(39, 1.0, 0.8549019607843137, 0.7254901960784313);
	color->AddRGBPoint(40, 0.9176470588235294, 0.6627450980392157, 0.11764705882352941);
	color->AddRGBPoint(41, 0.803921568627451, 0.0392156862745098, 0.49019607843137253);
	color->AddRGBPoint(42, 0.48627450980392156, 0.5490196078431373, 0.6980392156862745);
	color->AddRGBPoint(43, 0.8666666666666667, 0.8862745098039215, 0.26666666666666666);
	color->AddRGBPoint(44, 0.0, 0.19607843137254902, 0.5019607843137255);
	color->AddRGBPoint(45, 1.0, 0.8, 0.4);
	color->AddRGBPoint(46, 0.7843137254901961, 0.7843137254901961, 0.7843137254901961);
	color->AddRGBPoint(47, 0.7843137254901961, 0.7843137254901961, 0.7843137254901961);
	color->AddRGBPoint(48, 0.49019607843137253, 0.9803921568627451, 0.08235294117647059);
	color->AddRGBPoint(49, 0.49019607843137253, 0.9803921568627451, 0.08235294117647059);
	color->AddRGBPoint(50, 0.4, 0.6980392156862745, 1.0);
	color->AddRGBPoint(51, 0.4, 0.6980392156862745, 1.0);
	color->AddRGBPoint(52, 0.24705882352941178, 0.40784313725490196, 0.8784313725490196);
	color->AddRGBPoint(53, 0.24705882352941178, 0.40784313725490196, 0.8784313725490196);
	color->AddRGBPoint(54, 1.0, 0.09803921568627451, 0.4980392156862745);
	color->AddRGBPoint(55, 1.0, 0.09803921568627451, 0.4980392156862745);
	color->AddRGBPoint(56, 0.23529411764705882, 0.7411764705882353, 0.5098039215686274);
	color->AddRGBPoint(57, 0.23529411764705882, 0.7411764705882353, 0.5098039215686274);
	color->AddRGBPoint(58, 0.7372549019607844, 0.7098039215686275, 0.4117647058823529);
	color->AddRGBPoint(59, 0.7372549019607844, 0.7098039215686275, 0.4117647058823529);
	color->AddRGBPoint(60, 0.996078431372549, 0.8352941176470589, 0.0);
	color->AddRGBPoint(61, 0.996078431372549, 0.8352941176470589, 0.0);
	color->AddRGBPoint(62, 0.23529411764705882, 0.7058823529411765, 0.7058823529411765);
	color->AddRGBPoint(63, 0.23529411764705882, 0.7058823529411765, 0.7058823529411765);
	color->AddRGBPoint(64, 0.8, 0.4980392156862745, 0.0);
	color->AddRGBPoint(65, 0.8, 0.4980392156862745, 0.0);
	color->AddRGBPoint(66, 0.6862745098039216, 0.7607843137254902, 0.8666666666666667);
	color->AddRGBPoint(67, 0.6862745098039216, 0.7607843137254902, 0.8666666666666667);
	color->AddRGBPoint(68, 0.8823529411764706, 0.6549019607843137, 0.40784313725490196);
	color->AddRGBPoint(69, 0.8823529411764706, 0.6549019607843137, 0.40784313725490196);
	color->AddRGBPoint(70, 0.8980392156862745, 0.4980392156862745, 0.4980392156862745);
	color->AddRGBPoint(71, 0.8980392156862745, 0.4980392156862745, 0.4980392156862745);
	color->AddRGBPoint(72, 0.49019607843137253, 0.0196078431372549, 0.09803921568627451);
	color->AddRGBPoint(73, 0.09803921568627451, 0.39215686274509803, 0.1568627450980392);
	color->AddRGBPoint(74, 0.49019607843137253, 0.39215686274509803, 0.6274509803921569);
	color->AddRGBPoint(75, 0.39215686274509803, 0.09803921568627451, 0.0);
	color->AddRGBPoint(76, 0.8627450980392157, 0.0784313725490196, 0.39215686274509803);
	color->AddRGBPoint(77, 0.8627450980392157, 0.0784313725490196, 0.0392156862745098);
	color->AddRGBPoint(78, 0.7058823529411765, 0.8627450980392157, 0.5490196078431373);
	color->AddRGBPoint(79, 0.8627450980392157, 0.23529411764705882, 0.8627450980392157);
	color->AddRGBPoint(80, 0.7058823529411765, 0.1568627450980392, 0.47058823529411764);
	color->AddRGBPoint(81, 0.5490196078431373, 0.0784313725490196, 0.5490196078431373);
	color->AddRGBPoint(82, 0.0784313725490196, 0.11764705882352941, 0.5490196078431373);
	color->AddRGBPoint(83, 0.13725490196078433, 0.29411764705882354, 0.19607843137254902);
	color->AddRGBPoint(84, 0.8823529411764706, 0.5490196078431373, 0.5490196078431373);
	color->AddRGBPoint(85, 0.7843137254901961, 0.13725490196078433, 0.29411764705882354);
	color->AddRGBPoint(86, 0.6274509803921569, 0.39215686274509803, 0.19607843137254902);
	color->AddRGBPoint(87, 0.9882352941176471, 0.9882352941176471, 0.9882352941176471);
	color->AddRGBPoint(88, 0.0784313725490196, 0.8627450980392157, 0.23529411764705882);
	color->AddRGBPoint(89, 0.23529411764705882, 0.8627450980392157, 0.23529411764705882);
	color->AddRGBPoint(90, 0.8627450980392157, 0.7058823529411765, 0.5490196078431373);
	color->AddRGBPoint(91, 0.8627450980392157, 0.23529411764705882, 0.0784313725490196);
	color->AddRGBPoint(92, 0.47058823529411764, 0.39215686274509803, 0.23529411764705882);
	color->AddRGBPoint(93, 0.8627450980392157, 0.0784313725490196, 0.0784313725490196);
	color->AddRGBPoint(94, 0.8627450980392157, 0.7058823529411765, 0.8627450980392157);
	color->AddRGBPoint(95, 0.23529411764705882, 0.0784313725490196, 0.8627450980392157);
	color->AddRGBPoint(96, 0.6274509803921569, 0.5490196078431373, 0.7058823529411765);
	color->AddRGBPoint(97, 0.3137254901960784, 0.0784313725490196, 0.5490196078431373);
	color->AddRGBPoint(98, 0.29411764705882354, 0.19607843137254902, 0.49019607843137253);
	color->AddRGBPoint(99, 0.0784313725490196, 0.8627450980392157, 0.6274509803921569);
	color->AddRGBPoint(100, 0.0784313725490196, 0.7058823529411765, 0.5490196078431373);
	color->AddRGBPoint(101, 0.5490196078431373, 0.8627450980392157, 0.8627450980392157);
	color->AddRGBPoint(102, 0.3137254901960784, 0.6274509803921569, 0.0784313725490196);
	color->AddRGBPoint(103, 0.39215686274509803, 0.0, 0.39215686274509803);
	color->AddRGBPoint(104, 0.27450980392156865, 0.27450980392156865, 0.27450980392156865);
	color->AddRGBPoint(105, 0.5882352941176471, 0.5882352941176471, 0.7843137254901961);
	color->AddRGBPoint(106, 0.3137254901960784, 0.7686274509803922, 0.3843137254901961);
	color->AddRGBPoint(107, 0.09803921568627451, 0.39215686274509803, 0.1568627450980392);
	color->AddRGBPoint(108, 0.49019607843137253, 0.39215686274509803, 0.6274509803921569);
	color->AddRGBPoint(109, 0.39215686274509803, 0.09803921568627451, 0.0);
	color->AddRGBPoint(110, 0.8627450980392157, 0.0784313725490196, 0.39215686274509803);
	color->AddRGBPoint(111, 0.8627450980392157, 0.0784313725490196, 0.39215686274509803);
	color->AddRGBPoint(112, 0.7058823529411765, 0.8627450980392157, 0.5490196078431373);
	color->AddRGBPoint(113, 0.8627450980392157, 0.23529411764705882, 0.8627450980392157);
	color->AddRGBPoint(114, 0.7058823529411765, 0.1568627450980392, 0.47058823529411764);
	color->AddRGBPoint(115, 0.5490196078431373, 0.0784313725490196, 0.5490196078431373);
	color->AddRGBPoint(116, 0.0784313725490196, 0.11764705882352941, 0.5490196078431373);
	color->AddRGBPoint(117, 0.13725490196078433, 0.29411764705882354, 0.19607843137254902);
	color->AddRGBPoint(118, 0.8823529411764706, 0.5490196078431373, 0.5490196078431373);
	color->AddRGBPoint(119, 0.7843137254901961, 0.13725490196078433, 0.29411764705882354);
	color->AddRGBPoint(120, 0.6274509803921569, 0.39215686274509803, 0.19607843137254902);
	color->AddRGBPoint(121, 0.0784313725490196, 0.8627450980392157, 0.23529411764705882);
	color->AddRGBPoint(122, 0.23529411764705882, 0.8627450980392157, 0.23529411764705882);
	color->AddRGBPoint(123, 0.8627450980392157, 0.7058823529411765, 0.5490196078431373);
	color->AddRGBPoint(124, 0.0784313725490196, 0.39215686274509803, 0.19607843137254902);
	color->AddRGBPoint(125, 0.8627450980392157, 0.23529411764705882, 0.0784313725490196);
	color->AddRGBPoint(126, 0.47058823529411764, 0.39215686274509803, 0.23529411764705882);
	color->AddRGBPoint(127, 0.8627450980392157, 0.0784313725490196, 0.0784313725490196);
	color->AddRGBPoint(128, 0.8627450980392157, 0.7058823529411765, 0.8627450980392157);
	color->AddRGBPoint(129, 0.23529411764705882, 0.0784313725490196, 0.8627450980392157);
	color->AddRGBPoint(130, 0.6274509803921569, 0.5490196078431373, 0.7058823529411765);
	color->AddRGBPoint(131, 0.3137254901960784, 0.0784313725490196, 0.5490196078431373);
	color->AddRGBPoint(132, 0.29411764705882354, 0.19607843137254902, 0.49019607843137253);
	color->AddRGBPoint(133, 0.0784313725490196, 0.8627450980392157, 0.6274509803921569);
	color->AddRGBPoint(134, 0.0784313725490196, 0.7058823529411765, 0.5490196078431373);
	color->AddRGBPoint(135, 0.5490196078431373, 0.8627450980392157, 0.8627450980392157);
	color->AddRGBPoint(136, 0.3137254901960784, 0.6274509803921569, 0.0784313725490196);
	color->AddRGBPoint(137, 0.39215686274509803, 0.0, 0.39215686274509803);
	color->AddRGBPoint(138, 0.27450980392156865, 0.27450980392156865, 0.27450980392156865);
	color->AddRGBPoint(139, 0.5882352941176471, 0.5882352941176471, 0.7843137254901961);
	color->AddRGBPoint(140, 0.9019607843137255, 0.9803921568627451, 0.9019607843137255);
	color->AddRGBPoint(141, 0.9019607843137255, 0.6078431372549019, 0.8431372549019608);
	color->AddRGBPoint(142, 0.5098039215686274, 0.6078431372549019, 0.37254901960784315);
	color->AddRGBPoint(143, 0.6078431372549019, 0.9019607843137255, 1.0);
	color->AddRGBPoint(144, 0.9803921568627451, 0.9411764705882353, 0.0784313725490196);
	color->AddRGBPoint(145, 0.13725490196078433, 0.9215686274509803, 0.6078431372549019);
	color->AddRGBPoint(146, 0.29411764705882354, 0.13725490196078433, 0.45098039215686275);
	color->AddRGBPoint(147, 0.13725490196078433, 0.7647058823529411, 0.13725490196078433);
	color->AddRGBPoint(148, 0.9215686274509803, 0.8823529411764706, 0.45098039215686275);
	color->AddRGBPoint(149, 0.8627450980392157, 0.7058823529411765, 0.803921568627451);

	vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
	volumeProperty->ShadeOff();
	volumeProperty->SetInterpolationTypeToLinear();
	volumeProperty->SetAmbient(0.2);
	volumeProperty->SetDiffuse(0.9);
	volumeProperty->SetSpecular(0.2);
	volumeProperty->SetSpecularPower(20.0);

	volumeProperty->SetColor(color);
	volumeProperty->SetScalarOpacity(scalarOpacity);
	volumeProperty->SetGradientOpacity(gradientOpacity);
	volumeProperty->SetInterpolationTypeToLinear();

	vtkVolume *volume = vtkVolume::New();

	volume->SetMapper(mapperVolume);
	volume->SetProperty(volumeProperty);

	vtkRenderer	*renderer = vtkRenderer::New();
	renderer->SetBackground(0.1, 0.1, 0.2);

	vtkRenderWindow	*renderWindow = vtkRenderWindow::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(500, 500);
	renderer->AddVolume(volume);
	renderer->ResetCamera();

	vtkInteractorStyleTrackballCamera *interactorStyle = vtkInteractorStyleTrackballCamera::New();
	vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor::New();
	renderWindowInteractor->SetInteractorStyle(interactorStyle);
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderWindow->Render();
	renderWindowInteractor->Start();


    return;
}
