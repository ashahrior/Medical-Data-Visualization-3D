#include <iostream>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <dos.h>

#include "vtkVolumeRenderer.h"

#include "FixedPointVolumeRayCastMapperCT.h"

#include "ConfidenceConnected3D.h"

#include "readDICOMSeries.h"

#include "itkWaterShedCode.h"


string s;

void delay(string str)
{
    for(int i = 0; i<str.size(); i++)
    {
        //long int s = 10000000;
        //while(s!=0)
            //s--;
        cout<<str[i];
    }
    cout<<endl;
}


int main()
{
    while(1)
    {
        cout<< "\nSelect operation code:"<<endl;
        cout<< "Code 1: Volume Rendering for Brain Data"<<endl;
        cout<< "Code 2: Volume Rendering for CT Bone Data" <<endl;
        cout<< "Code 3: Medical Data Watershed Segmentation" <<endl;
        cout<< "Code 4: DICOM file Series Viewer"<<endl;
        cout<< "Code 5: Confidence Connected Segmentation of Brain White Matter" <<endl;
		cout << "Type E/e to Exit" << endl;
        char c;
		cout << ">> ";
        cin>>c;
		if (c == 'E' || c=='e')
			return EXIT_SUCCESS;

        char dir[1000], type[100];
        getchar();
        switch(c)
        {
        case '1':
            s = "Volume Rendering for Brain Data";
            delay(s);
            printf("Enter directory/filepath file_type[ '-FILE / -DIR' ]: ");
            cin>>dir>>type;
            renderBrain(dir,type);
            break;
        case '2':
            s = "Volume Rendering for CT Bone Data";
            delay(s);
            renderBone();
            break;
        case '3':
            s = "Medical Data Watershed Segmentation";
            delay(s);
            printf("Enter file path: ");
            char file_name[1000];
            cin>>file_name;
            watershedSegmentation(file_name);
            break;
        case '4':
            s = "DICOM file Series Viewer";
            delay(s);
            printf("Enter directory path: ");
            cin>>dir;
            showSeries(dir);
            break;
        case '5':
            s = "Confidence Connected Segmentation of Brain White Matter";
            delay(s);
            confidenceSeg();
            break;
        }
    }
    return EXIT_SUCCESS;
}
