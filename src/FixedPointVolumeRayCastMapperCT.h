#include <vtkFixedPointVolumeRayCastMapper.h>

#include <vtkBoxWidget.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkColorTransferFunction.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>
#include <vtkImageResample.h>
#include <vtkMetaImageReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPlanes.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkXMLImageDataReader.h>
#include <vtkNamedColors.h>

#define MHA_FILETYPE 2

namespace
{
void PrintUsage()
{
    cout << "Usage: " << endl;
    cout << endl;
    cout << "  FixedPointVolumeRayCastMapper for Bone CT Visualization <options>" << endl;
    cout << endl;
    cout << "where options may include: " << endl;
    cout << endl;
    cout << "  -DICOM <directory>" << endl;
    cout << "  -MHA <filename>" << endl;
    cout << "  -CT_Bone" << endl;
    cout << "You must use either the -DICOM option to specify the directory where" << endl;
    cout << "the data is located or the -MHA option to specify the path of a .mha/.mhd file." << endl;
    cout << endl;
    cout << "Example: -DICOM Head" << endl;
    cout << endl;
}
}
void renderBone()
{

	PrintUsage();

    int count = 1;
    char *dirname = NULL;
    double opacityWindow = 4096;
    double opacityLevel = 2048;
	int blendType = 4;
    int clip = 0;
    double reductionFactor = 1.0;
    double frameRate = 10.0;
    char *fileName=0;
    int fileType=0;

    bool independentComponents=true;

	cout << "File/Directory? '-MHA / -DICOM': ";
	char choice[100];
	cin >> choice;
	cout << "Enter path: ";
	char path[1000];
	cin >> path;
	if (!strcmp(choice, "-DICOM"))
	{
		size_t size = strlen(path) + 1;
		dirname = new char[size];
		snprintf(dirname, size, "%s", path);
		count += 2;
	}
	else if (!strcmp(choice, "-MHA"))
	{
		size_t size = strlen(path) + 1;
		fileName = new char[size];
		fileType = MHA_FILETYPE;
		snprintf(fileName, size, "%s", path);
		count += 2;
	}
	else
	{
		cout << "Unrecognized option: " << choice << endl;
		cout << endl;
		PrintUsage();
		return;
	}

    if ( !dirname && !fileName)
    {
        cout << "Error: you must specify a directory of DICOM data or a .mha!" << endl;
        cout << endl;
        PrintUsage();
        return;
    }

    vtkNamedColors *colors = vtkNamedColors::New();
    vtkRenderer *renderer = vtkRenderer::New();
    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);


    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);
    iren->SetDesiredUpdateRate(frameRate / (1+clip) );

    iren->GetInteractorStyle()->SetDefaultRenderer(renderer);

    vtkAlgorithm *reader=0;
    vtkImageData *input=0;
    if(dirname)
    {
        vtkDICOMImageReader *dicomReader = vtkDICOMImageReader::New();
        dicomReader->SetDirectoryName(dirname);
        dicomReader->Update();
        input=dicomReader->GetOutput();
        reader=dicomReader;
    }
    else if ( fileType == MHA_FILETYPE )
    {
        vtkMetaImageReader *metaReader = vtkMetaImageReader::New();
        metaReader->SetFileName(fileName);
        metaReader->Update();
        input=metaReader->GetOutput();
        reader=metaReader;
    }
    else
    {
        cout << "Error! Not MHA!" << endl;
        return;
    }

    int dim[3];
    input->GetDimensions(dim);
    if ( dim[0] < 2 || dim[1] < 2 || dim[2] < 2 )
    {
        cout << "Error loading data!" << endl;
        return;
    }

    vtkVolume *volume = vtkVolume::New();
    vtkFixedPointVolumeRayCastMapper *mapper = vtkFixedPointVolumeRayCastMapper::New();

    mapper->SetInputConnection( reader->GetOutputPort() );

    vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
    vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();

    vtkVolumeProperty *property = vtkVolumeProperty::New();
    property->SetIndependentComponents(independentComponents);
    property->SetColor( colorFun );
    property->SetScalarOpacity( opacityFun );
    property->SetInterpolationTypeToLinear();

    volume->SetProperty( property );
    volume->SetMapper( mapper );

	colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
	colorFun->AddRGBPoint(-16, 0.73, 0.25, 0.30, 0.49, .61);
	colorFun->AddRGBPoint(641, .90, .82, .56, .5, 0.0);
	colorFun->AddRGBPoint(3071, 1, 1, 1, .5, 0.0);

	opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
	opacityFun->AddPoint(-16, 0, .49, .61);
	opacityFun->AddPoint(641, .72, .5, 0.0);
	opacityFun->AddPoint(3071, .71, 0.5, 0.0);

	mapper->SetBlendModeToComposite();
	property->ShadeOn();
	property->SetAmbient(0.1);
	property->SetDiffuse(0.9);
	property->SetSpecular(0.2);
	property->SetSpecularPower(10.0);
	property->SetScalarOpacityUnitDistance(0.8919);

    renWin->SetSize(600,600);
    renWin->Render();

    renderer->AddVolume( volume );

    renderer->ResetCamera();
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renWin->Render();

    iren->Start();

    opacityFun->Delete();
    colorFun->Delete();
    property->Delete();

    volume->Delete();
    mapper->Delete();
    reader->Delete();
    renderer->Delete();
    renWin->Delete();
    iren->Delete();

    return;
}
