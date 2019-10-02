#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>

#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>

#include <sstream>

using namespace std;

class StatusMessage
{
public:
    static string Format(int slice, int maxSlice)
    {
        stringstream tmp;
        tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
        return tmp.str();
    }
};


class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
    static myVtkInteractorStyleImage* New();
    vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);

protected:
    vtkImageViewer2* _ImageViewer;
    vtkTextMapper* _StatusMapper;
    int _Slice;
    int _MinSlice;
    int _MaxSlice;

public:
    void SetImageViewer(vtkImageViewer2* imageViewer)
    {
        _ImageViewer = imageViewer;
        _MinSlice = imageViewer->GetSliceMin();
        _MaxSlice = imageViewer->GetSliceMax();
        _Slice = _MinSlice;
        //cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
    }

    void SetStatusMapper(vtkTextMapper* statusMapper)
    {
        _StatusMapper = statusMapper;
    }


protected:
    void MoveSliceForward()
    {
        if(_Slice < _MaxSlice)
        {
            _Slice += 1;
            //cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
            _ImageViewer->SetSlice(_Slice);
            string msg = StatusMessage::Format(_Slice, _MaxSlice);
            _StatusMapper->SetInput(msg.c_str());
            _ImageViewer->Render();
        }
    }

    void MoveSliceBackward()
    {
        if(_Slice > _MinSlice)
        {
            _Slice -= 1;
            //cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
            _ImageViewer->SetSlice(_Slice);
            std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
            _StatusMapper->SetInput(msg.c_str());
            _ImageViewer->Render();
        }
    }


    virtual void OnKeyDown()
    {
        std::string key = this->GetInteractor()->GetKeySym();
        if(key.compare("Up") == 0)
        {
            //cout << "Up arrow key was pressed." << endl;
            MoveSliceForward();
        }
        else if(key.compare("Down") == 0)
        {
            //cout << "Down arrow key was pressed." << endl;
            MoveSliceBackward();
        }
        // forward event
        vtkInteractorStyleImage::OnKeyDown();
    }


    virtual void OnMouseWheelForward()
    {
        //std::cout << "Scrolled mouse wheel forward." << std::endl;
        MoveSliceForward();

    }


    virtual void OnMouseWheelBackward()
    {
        //std::cout << "Scrolled mouse wheel backward." << std::endl;
        if(_Slice > _MinSlice)
        {
            MoveSliceBackward();
        }

    }
};

vtkStandardNewMacro(myVtkInteractorStyleImage);


void showSeries(char *dir)
{

    string folder = dir;

    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDirectoryName(folder.c_str());
    reader->Update();

    vtkSmartPointer<vtkImageViewer2> imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
    imageViewer->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkTextProperty> sliceTextProp = vtkSmartPointer<vtkTextProperty>::New();
    sliceTextProp->SetFontFamilyToCourier();
    sliceTextProp->SetFontSize(20);
    sliceTextProp->SetVerticalJustificationToBottom();
    sliceTextProp->SetJustificationToLeft();

    vtkSmartPointer<vtkTextMapper> sliceTextMapper = vtkSmartPointer<vtkTextMapper>::New();
    string msg = StatusMessage::Format(imageViewer->GetSliceMin(), imageViewer->GetSliceMax());
    sliceTextMapper->SetInput(msg.c_str());
    sliceTextMapper->SetTextProperty(sliceTextProp);

    vtkSmartPointer<vtkActor2D> sliceTextActor = vtkSmartPointer<vtkActor2D>::New();
    sliceTextActor->SetMapper(sliceTextMapper);
    sliceTextActor->SetPosition(15, 10);

    vtkSmartPointer<vtkTextProperty> usageTextProp = vtkSmartPointer<vtkTextProperty>::New();
    usageTextProp->SetFontFamilyToCourier();
    usageTextProp->SetFontSize(14);
    usageTextProp->SetVerticalJustificationToTop();
    usageTextProp->SetJustificationToLeft();

    vtkSmartPointer<vtkTextMapper> usageTextMapper = vtkSmartPointer<vtkTextMapper>::New();
    usageTextMapper->SetInput("- Slice with mouse wheel\n  or Up/Down-Key\n- Zoom with pressed right\n  mouse button while dragging");
    usageTextMapper->SetTextProperty(usageTextProp);

    vtkSmartPointer<vtkActor2D> usageTextActor = vtkSmartPointer<vtkActor2D>::New();
    usageTextActor->SetMapper(usageTextMapper);
    usageTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
    usageTextActor->GetPositionCoordinate()->SetValue( 0.05, 0.95);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    vtkSmartPointer<myVtkInteractorStyleImage> myInteractorStyle = vtkSmartPointer<myVtkInteractorStyleImage>::New();


    myInteractorStyle->SetImageViewer(imageViewer);
    myInteractorStyle->SetStatusMapper(sliceTextMapper);

    imageViewer->SetupInteractor(renderWindowInteractor);

    renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

    imageViewer->GetRenderer()->AddActor2D(sliceTextActor);
    imageViewer->GetRenderer()->AddActor2D(usageTextActor);


    imageViewer->Render();
    imageViewer->GetRenderer()->ResetCamera();
    imageViewer->Render();
    renderWindowInteractor->Start();
    return;
}
